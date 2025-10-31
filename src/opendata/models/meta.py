from model import Model


class Frame(Model):
    x: int
    y: int
    w: int
    h: int
    duration: int


class Size(Model):
    w: int
    h: int


class Animation(Model):
    name: str
    frames: list[Frame]


class Sprite(Model):
    name: str
    size: Size
    animations: list[Animation]


class Data(Model):
    sprites: list[Sprite]
