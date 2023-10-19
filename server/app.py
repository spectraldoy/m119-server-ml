from flask import Flask, request
from model import get_model
import torch

model = get_model()

app = Flask(__name__)

@app.route("/")
def hello():
    return "Hello!"

@app.route("/query", methods=["GET"])
def inference():
    ax = float(request.args.get("ax", 0))
    ay = float(request.args.get("ay", 0))
    az = float(request.args.get("az", 0))

    output = model(torch.tensor([ax, ay, az], device=torch.device("cpu"))).max()
    if az > 0:
        return "up"
    else:
        return "down"

