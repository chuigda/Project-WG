import time
import json
from asyncio import CancelledError, create_task, sleep as async_sleep
from websockets.client import connect as connect_ws
from websockets.exceptions import ConnectionClosed

class WebsocketWorker:
    def __init__(self):
        self.async_task = None

    def connect_read(self, url, data_callback, error_callback, done_callback, log):
        self.async_task = create_task(self.connect_read_impl(
            url,
            data_callback,
            error_callback,
            done_callback,
            log
        ))

    async def connect_read_impl(self, url, data_callback, error_callback, done_callback, log):
        log("Connecting to VTS server at %s ..." % url)
        try:
            async with connect_ws(url) as ws:
                await ws.send("""
{
  "apiName": "VTubeStudioPublicAPI",
  "apiVersion": "1.0",
  "requestID": "StartCommunicationAuthRequest",
  "messageType": "AuthenticationTokenRequest",
  "data": {
    "pluginName": "DSYS - Flight Recorder",
    "pluginDeveloper": "Chuigda WhiteGive"
  }
}
""".strip())
                resp = json.loads(await ws.recv())
                if resp["requestID"] != "StartCommunicationAuthRequest" \
                   or resp["messageType"] != "AuthenticationTokenResponse":
                    raise Exception("*** ERROR: AUTHENTICATION FAILED")
                auth_token = resp["data"]["authenticationToken"]

                await ws.send("""
{
    "apiName": "VTubeStudioPublicAPI",
    "apiVersion": "1.0",
    "requestID": "AuthenticationRequest",
    "messageType": "AuthenticationRequest",
    "data": {
        "pluginName": "DSYS - Flight Recorder",
        "pluginDeveloper": "Chuigda WhiteGive",
        "authenticationToken": "%s"
    }
}
""".strip() % auth_token)
                resp = json.loads(await ws.recv())
                if resp["requestID"] != "AuthenticationRequest" \
                     or resp["messageType"] != "AuthenticationResponse" \
                     or resp["data"]["authenticated"] != True:
                      raise Exception("*** ERROR: AUTHENTICATION FAILED")
                
                request_id = 0
                while True:
                    start_time = time.time()
                    request_id += 1
                    await ws.send("""
{
    "apiName": "VTubeStudioPublicAPI",
    "apiVersion": "1.0",
    "requestID": "GetLive2DModelRequest_%d",
    "messageType": "InputParameterListRequest",
    "data": {
        "authenticationToken": "%s",
    }
}
""".strip() % (request_id, auth_token))
                    resp = json.loads(await ws.recv())
                    if resp["messageType"] != "InputParameterListResponse":
                        raise Exception("*** ERROR: RECV FAILED")
                    data_callback(resp["data"])
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
            error_callback()
        finally:
            done_callback()
            self.async_task = None

    def stop_task(self):
        if self.async_task:
            self.async_task.cancel()
            self.async_task = None
