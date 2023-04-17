import time
import json
import traceback
from asyncio import CancelledError, create_task, sleep as async_sleep
from websockets.client import connect as connect_ws
from websockets.exceptions import ConnectionClosed
from metadata import Icon_Base64
from util import json_stringify
from bin_rw import read_binseq, write_binseq
from compdec import compress, decompress


class WebsocketWorker:
    def __init__(self):
        self.async_task = None
        self.token_dict = {}

    def connect_read(self, url, save_file, error_callback, done_callback, log):
        if self.async_task is not None:
            log("*** ERROR: another task is running, this should be a system bug")
            log("*** INFO: try RST the system")
            return
        self.async_task = create_task(self.connect_read_impl(
            url,
            save_file,
            error_callback,
            done_callback,
            log
        ))

    async def connect_read_impl(self, url, save_file, error_callback, done_callback, log):
        log("Connecting to VTS server at %s ..." % url)
        try:
            async with connect_ws(url) as ws:
                auth_token = await self.establish_battlefield_control(ws, log, "DSYS - Flight Recorder")

                log("Authenticated, commencing tracking operation")
                request_id = 0
                with open(save_file, "wb") as f:
                    data_lines = []
                    while True:
                        start_time = time.time()
                        request_id += 1
                        await ws.send(json_stringify({
                            "apiName": "VTubeStudioPublicAPI",
                            "apiVersion": "1.0",
                            "requestID": "GetLive2DModelRequest_%d" % request_id,
                            "messageType": "InputParameterListRequest",
                            "data": {
                                "authenticationToken": auth_token
                            }
                        }))
                        resp = json.loads(await ws.recv())
                        if resp["messageType"].lower().find("error") != -1:
                            raise Exception("*** ERROR: Received error response")
                        elif resp["messageType"] != "InputParameterListResponse":
                            continue
                        data_lines.append(json_stringify(resp["data"]))
                        if len(data_lines) >= 50:
                            data_text = "\n".join(data_lines)
                            compressed = compress(data_text)
                            write_binseq(f, compressed)
                            data_lines = []
                        end_time = time.time()
                        time_elapsed = end_time - start_time
                        if time_elapsed >= 0.04:
                            log("*** WARNING: Frame took too long to process: %fms" % (time_elapsed * 1000.0))
                        else:
                            await async_sleep(0.04 - time_elapsed)
        except ConnectionClosed as e:
            log("*** WARN: Connection closed, VTS probably closed")
            error_callback()
        except CancelledError as e:
            log("*** INFO: Task cancelled due to system RST")
        except Exception as e:
            log("*** ERROR: Exception in task: %s: %s" % (e.__class__.__name__, e.__str__()))
            traceback.print_exc()
            error_callback()
        finally:
            done_callback()
            self.async_task = None

    def connect_send(self, url, data_source, error_callback, done_callback, log, filt=True, cyc=False):
        if self.async_task is not None:
            log("*** ERROR: another task is running, this should be a system bug")
            log("*** INFO: try RST the system")
            return
        self.async_task = create_task(self.connect_send_impl(
            url,
            data_source,
            error_callback,
            done_callback,
            log,
            filt,
            cyc
        ))

    async def connect_send_impl(self, url, data_source, error_callback, done_callback, log, filt, cyc):
        log("Connecting to VTS server at %s ..." % url)
        try:
            async with connect_ws(url) as ws:
                auth_token = await self.establish_battlefield_control(ws, log, "DSYS - Dummy Entry Plug")

                log("Authenticated, commencing RPL/AP operation")
                request_id = 0
                data_lines = []
                while True:
                    with open(data_source, "rb") as f:
                        while True:
                            start_time = time.time()
                            if len(data_lines) == 0:
                                result = read_binseq(f)
                                if result is None:
                                    break
                                data_lines = decompress(result).split('\n')
                            line = data_lines.pop(0)
                            json_data = json.loads(line)
                            request_id += 1
                            data = [{
                                "id": param["name"],
                                "value": param["value"]
                            } for param in json_data["defaultParameters"]]
                            if filt:
                                data = [param for param in data if filter_param(param)]
                            await ws.send(json_stringify({
                                "apiName": "VTubeStudioPublicAPI",
                                "apiVersion": "1.0",
                                "requestID": "InjectParameterDataRequest_%d" % request_id,
                                "messageType": "InjectParameterDataRequest",
                                "data": {
                                    "authenticationToken": auth_token,
                                    "mode": "set",
                                    "parameterValues": data
                                }
                            }))
                            resp = json.loads(await ws.recv())
                            if resp["messageType"].lower().find("error") != -1:
                                raise Exception("*** ERROR: Received error response")
                            end_time = time.time()
                            time_elapsed = end_time - start_time
                            if time_elapsed >= 0.04:
                                log("*** WARNING: Frame took too long to process: %fms" % (time_elapsed * 1000.0))
                            else:
                                await async_sleep(0.04 - time_elapsed)
                    # inner loop ends here
                    if not cyc:
                        break
        except ConnectionClosed as e:
            log("*** WARN: Connection closed, VTS probably closed")
            error_callback()
        except CancelledError as e:
            log("*** INFO: Task cancelled due to system RST")
        except Exception as e:
            log("*** ERROR: Exception in task: %s: %s" % (e.__class__.__name__, e.__str__()))
            traceback.print_exc()
            error_callback()
        finally:
            done_callback()
            self.async_task = None

    async def establish_battlefield_control(self, ws, log, plugin_name):
        auth_token = self.token_dict.get(plugin_name)
        if auth_token is not None:
            log("Using saved authentication token")
        else:
            log("Requesting authentication token, please check VTS window")
            await ws.send(json_stringify({
                "apiName": "VTubeStudioPublicAPI",
                "apiVersion": "1.0",
                "requestID": "AuthenticationTokenRequest",
                "messageType": "AuthenticationTokenRequest",
                "data": {
                    "pluginName": plugin_name,
                    "pluginDeveloper": "Chuigda WhiteGive",
                    "pluginIcon": Icon_Base64
                }
            }))
            resp = json.loads(await ws.recv())
            if resp["requestID"] != "AuthenticationTokenRequest" \
                    or resp["messageType"] != "AuthenticationTokenResponse":
                raise Exception("*** ERROR: AUTHENTICATION FAILED")
            auth_token = resp["data"]["authenticationToken"]
            self.token_dict[plugin_name] = auth_token

        log("Authentication token received, authenticating")
        await ws.send(json_stringify({
            "apiName": "VTubeStudioPublicAPI",
            "apiVersion": "1.0",
            "requestID": "AuthenticationRequest",
            "messageType": "AuthenticationRequest",
            "data": {
                "pluginName": plugin_name,
                "pluginDeveloper": "Chuigda WhiteGive",
                "authenticationToken": auth_token
            }
        }))
        resp = json.loads(await ws.recv())
        if resp["requestID"] != "AuthenticationRequest" \
                or resp["messageType"] != "AuthenticationResponse" \
                or resp["data"]["authenticated"] != True:
            raise Exception("*** ERROR: AUTHENTICATION FAILED")
        return auth_token

    def stop_task(self):
        if self.async_task:
            self.async_task.cancel()
            self.async_task = None

    def reset_tokens(self):
        self.token_dict = {}


Mouth_Related_Param = {
    "MouthSmile",
    "MouthOpen",
    "VoiceVolume",
    "VoiceFrequency",
    "VoiceVolumePlusMouthOpen",
    "VoiceFrequencyPlusMouthSmile"
}


def filter_param(param):
    if param["id"] in Mouth_Related_Param:
        return False
    return True
