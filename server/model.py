import torch
from torch import nn

def get_model():
    model = nn.Sequential(
        nn.Linear(3, 10),
        nn.ReLU(),
        nn.Linear(10, 2),
        nn.Sigmoid()
    ).to(torch.device("cpu"))
    return model
