import asyncio
import logging
import json
from typing import KeysView, OrderedDict
from enum import Enum

class GameControlMessages(Enum):
    PAUSE=0
    RESUME=1
    END_ROUND=3
    START_ROUND=4

class GameControlState(Enum):
    STOPPED=0
    RUNNING=1


class IncommingMessage(object):
    adcs_alive = True
    alive_lock = asyncio.Lock()
    poke = 0

    def __init__(self, name: str) -> None:
        # self.logger = logger
        self.element_store = dict()
        self.name = name
        self.sensor_publish_time = 0.0
        self.time_previous = 0.0
        self.time_str = None
        self.time_num = 0.0
        self.deltatime = None
        self.reset()

    def process(self, message: str):
        # print(message)
        self.lines = message.rstrip("\0").splitlines()[:-3]
        self.sim_messeage_dict = OrderedDict()
        for line in self.lines:
            sim_message_key, sim_message_element = self.create_sim_message(line)
            self.sim_messeage_dict[sim_message_key] = sim_message_element
        self.time_str = self.sim_messeage_dict.get("SC[0].AC.Time", None)
        self.time_num = float(self.time_str)
        if "SC[0].AC.DT" in self.sim_messeage_dict.keys():
            self.deltatime = float(self.sim_messeage_dict.get("SC[0].AC.DT"))

    @staticmethod
    def create_sim_message(line: str):
        if line.startswith("TIME"):
            parts = line.split(" ")
        else:
            parts = line.split("=")
        message_key = parts[0].strip()
        message_elements = parts[1].strip().split(" ")
        message_elements = message_elements if len(message_elements) > 1 else message_elements[0]
        return (message_key, message_elements)

    def store_element(self, element_lookup: str):
        if element_lookup not in self.element_store.keys():
            self.element_store[element_lookup] = []
        self.element_store[element_lookup].append(self.sim_messeage_dict.get(element_lookup))

    def get_sim_message_json(self):
        return json.dumps(self.sim_messeage_dict, indent=4)

    def reset(self):
        self.time_previous = self.time_num
        self.lines = None
        self.sim_messeage_dict = None

    @property
    def ready(self):
        return True if self.sim_messeage_dict is not None else False