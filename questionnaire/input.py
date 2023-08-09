import inspect
from enum import StrEnum
import click
from json import JSONEncoder
from collections import namedtuple
from dataclasses import dataclass, asdict, fields, is_dataclass


def choice(options):
    click.echo(options.__name__ + '?')
    for (i, option) in enumerate(options):
        click.echo(f"{i}. ({option.name}) {option.value} ")
    index = click.prompt(f"Enter #", type=int, default=0, show_default=True)
    try:
        option = [option for (i, option) in enumerate(options)][index]
        return option
    except IndexError:
        click.echo("Invalid input")
        choice(options)


def generate_questionnaire(obj):
    if isinstance(obj, list):
        for item in obj:
            generate_questionnaire(item)
    elif is_dataclass(obj):
        for f in list(filter(lambda _f: True if not _f.metadata else not _f.metadata["skip"], fields(obj))):
            name = f.name
            val = getattr(obj, f.name)
            if isinstance(val, StrEnum):
                answer = choice(val.__class__)
                setattr(obj, name, answer)
            elif isinstance(val, (int, float, str)):
                question = f"{name}?"
                answer = click.prompt(question, type=type(val), default=val, show_default=True)
                setattr(obj, name, answer)
            else:
                click.echo(f"{name}.")
                generate_questionnaire(val)
    else:
        print("Data is corrupt!")


class DictJSONEncoder(JSONEncoder):
    def default(self, o):
        return o.__dict__


def dictDecoder(dict):
    return namedtuple('X', dict.keys())(*dict.values())
