from __future__ import annotations
from enum import StrEnum
from dataclasses import dataclass, field, fields, asdict
from typing import List
import datetime
from pathlib import Path
import yaml


class Meta:
    @dataclass
    class _FieldMetadata:
        skip: bool
    now = datetime.datetime.now()
    timestamp = now.strftime("%Y-%m-%d_%H-%M-%S")
    date = now.strftime("%Y-%m-%d")
    skip = asdict(_FieldMetadata(skip=True))


@dataclass
class Experiment:
    when: str = field(metadata=Meta.skip)
    where: Where
    # _gitInfo: GitInfo
    intent: Intent
    lpsCount: int
    lpsInfos: List[LPSInfo]
    cfInfo: CFInfo
    samplingInfo: SamplingInfo

    def __getstate__(self):
        state = self.__dict__.copy()
        del state["when"]
        return state

    def __setstate__(self, state):
        self.__dict__.update(state)
        self.when = Meta.timestamp





class Where(StrEnum):
    LAB_DE0302 = 'lab_de0302'


@dataclass
class Firmware:
    isStock: bool
    name: str
    notes: str


class Intent(StrEnum):
    NORMAL = 'normal',
    SPOOF = 'spoof'


class Estimator(StrEnum):
    EKF = 'ekf'


class Mode(StrEnum):
    TDOA2 = 'tdoa2',
    TDOA3 = 'tdoa3'


class Deck(StrEnum):
    LOCO = 'loco',
    ACTIVE_MARKER = 'active marker'


class DeckOptions(StrEnum):
    OPTION1 = ','.join([Deck.LOCO])
    OPTION2 = ','.join([Deck.LOCO, Deck.ACTIVE_MARKER])


@dataclass
class CFInfo:
    fw: Firmware
    isAtRest: bool
    estimator: Estimator
    deckOption: DeckOptions
    notes: str


class WhichLPS(StrEnum):
    OUTER = 'outer',
    INNER = 'inner'


@dataclass
class Node:
    id: int
    x: float
    y: float
    z: float


@dataclass
class LPSInfo:
    fw: Firmware
    mode: Mode
    which: WhichLPS
    yaml_file: str
    nodes: [Node] = field(init=False, metadata=Meta.skip)

    def __setattr__(self, name, value):
        if name == 'yaml_file':
            if Path(value).exists():
                with open(value, 'r') as file:
                    objs = yaml.load(file, Loader=yaml.FullLoader)
                    self.nodes = [Node(key, value['x'], value['y'], value['z']) for key, value in objs.items()]
        super().__setattr__(name, value)


@dataclass
class SamplingInfo:
    delay: int
    count: int
    rate: int
    actionsOption: ActionOptions

    def getTotalDuration(self) -> int:
        return self.count * self.rate


class Action(StrEnum):
    TURN_OFF_EVERYTHING = 'turn off everything',
    TURN_ON_MASTER_NORMAL_LPS = 'turn on master in normal lps',
    TURN_ON_SLAVES_NORMAL_LPS = 'turn on slaves in normal lps',
    TURN_ON_MASTER_ATTACK_LPS = 'turn on master in attack lps',
    TURN_ON_SLAVES_ATTACK_LPS = 'turn on slaves in attack lps',
    CONNECT_PC_CF = 'connect pc-cf'
    REMOTELY_RESET_CF = 'remotely reset cf',
    MANUALLY_RESET_CF = 'manually reset cf',
    ADD_DELAY = 'add delay',
    NO_DELAY = 'no delay',
    NAT_DELAY = 'natural delay'
    START_SAMPLING = 'start sampling'


class ActionOptions(StrEnum):
    OPTION1 = ' -> '.join(
        [Action.TURN_OFF_EVERYTHING,
         Action.TURN_ON_SLAVES_NORMAL_LPS,
         Action.TURN_ON_MASTER_NORMAL_LPS,
         Action.REMOTELY_RESET_CF,
         Action.START_SAMPLING])


@dataclass
class GitInfo:
    latest_commit: str
    current_branch: str
    latest_tag: str
    commit_message: str
    author: str
    timestamp: str
