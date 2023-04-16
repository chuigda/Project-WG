import sys
import traceback
import json

import torch
from torch import nn, optim
from bin_rw import read_binseq
from compdec import decompress


class NeuralNetwork(nn.Module):
    def __init__(self):
        super().__init__()
        self.linear_relu_stack = nn.Sequential(
            nn.Linear(2, 4),
            nn.Sigmoid(),
            nn.Linear(4, 4),
            nn.Sigmoid(),
            nn.Linear(4, 4),
            nn.Sigmoid(),
            nn.Linear(4, 2),
            nn.Sigmoid()
        )

    def forward(self, x):
        logits = self.linear_relu_stack(x)
        return logits


def get_array_item_by_name(arr, name):
    for item in arr:
        if item["name"] == name:
            return item.get("value")
    return None


def load_dataset(file_name: str):
    input_set = []
    output_set = []
    with open(file_name, "rb") as f:
        while True:
            result = read_binseq(f)
            if result is None:
                break
            chunk = decompress(result).split("\n")
            for line in chunk:
                data = json.loads(line)
                parameters = data["defaultParameters"]
                in_voice_volume = get_array_item_by_name(parameters, "VoiceVolume")
                in_voice_freq = get_array_item_by_name(parameters, "VoiceFrequency")
                out_mouth_open = get_array_item_by_name(parameters, "MouthOpen")
                out_mouth_smile = get_array_item_by_name(parameters, "MouthSmile")
                if in_voice_volume is None \
                        or in_voice_freq is None \
                        or out_mouth_smile is None \
                        or out_mouth_open is None:
                    continue
                input_set.append([in_voice_volume, in_voice_freq])
                output_set.append([out_mouth_open, out_mouth_smile])
    return input_set, output_set


Learning_Rate = 1e-2


def main():
    if len(sys.argv) != 2:
        print("Usage: python train.py <tck-file>", file=sys.stderr)
        sys.exit(1)

    try:
        print("Loading dataset from %s" % sys.argv[1])
        input_set, output_set = load_dataset(sys.argv[1])
        input_set = [torch.Tensor(x) for x in input_set]
        output_set = [torch.Tensor(x) for x in output_set]
        print("%d data lines (~%.3f seconds) data loaded" % (len(input_set), len(input_set) / 25.0))

        model = NeuralNetwork().to("cpu")
        criterion = nn.CrossEntropyLoss()
        optimizer = optim.SGD(model.parameters(), lr=Learning_Rate)
        for epoch in range(10):
            print("Running epoch %d" % (epoch + 1))
            running_loss = 0.0
            for i in range(len(input_set)):
                optimizer.zero_grad()
                outputs = model(input_set[i])
                loss = criterion(outputs, output_set[i])
                loss.backward()
                optimizer.step()
                running_loss += loss.item()

            print("Epoch %d loss: %.3f" % (epoch + 1, running_loss / len(input_set)))
    except Exception as e:
        print("*** ERROR: %s: %s" % (e.__class__.__name__, e), file=sys.stderr)
        traceback.print_exc()
        sys.exit(1)


if __name__ == "__main__":
    main()
