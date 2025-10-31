from model import Model


class XYWH(Model):
    x: int
    y: int
    w: int
    h: int


class WH(Model):
    w: int
    h: int


class Frame(Model):
    filename: str
    frame: XYWH
    rotated: bool
    trimmed: bool
    sprite_source_size: XYWH
    source_size: WH
    duration: int


class Meta(Model):
    app: str
    version: str
    image: str
    format: str
    size: WH
    scale: str


class Data(Model):
    frames: list[Frame]
    meta: Meta
