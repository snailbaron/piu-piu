#!/usr/bin/env python3

import argparse
import dataclasses
from pathlib import Path

import yaml

import aseprite
import meta


@dataclasses.dataclass
class Frame:
    x: int
    y: int
    w: int
    h: int
    duration: int


@dataclasses.dataclass
class Animation:
    name: str = ""
    w: int = 0
    h: int = 0
    frames: list[Frame] = dataclasses.field(default_factory=list[Frame])


@dataclasses.dataclass
class MutableData:
    w: int | None = None
    h: int | None = None
    animations: dict[str, list[Frame]] = dataclasses.field(
        default_factory=dict[str, list[Frame]]
    )


def collect_data_from_aseprite(data: aseprite.Data) -> dict[str, MutableData]:
    mutable_data: dict[str, MutableData] = {}

    for part_data in data.frames:
        filename_parts = part_data.filename.split(":")
        assert len(filename_parts) == 3

        sprite_name = filename_parts[0]
        animation_name = filename_parts[1]
        frame_index = int(filename_parts[2])

        sprite_w = part_data.source_size.w
        sprite_h = part_data.source_size.h
        frame = Frame(
            x=part_data.frame.x,
            y=part_data.frame.y,
            w=part_data.frame.w,
            h=part_data.frame.h,
            duration=part_data.duration,
        )

        if sprite_name not in mutable_data:
            mutable_data[sprite_name] = MutableData(w=sprite_w, h=sprite_h)
        else:
            assert mutable_data[sprite_name].w == sprite_w
            assert mutable_data[sprite_name].h == sprite_h
        sprite_data: MutableData = mutable_data[sprite_name]

        animation_data = sprite_data.animations.setdefault(animation_name, [])
        assert frame_index == len(animation_data)
        animation_data.append(frame)

    return mutable_data


def convert_to_meta(mutable_data: dict[str, MutableData]) -> meta.Data:
    sprites: list[meta.Sprite] = []
    for sprite_name, mutable_sprite in mutable_data.items():
        assert mutable_sprite.w is not None
        assert mutable_sprite.h is not None

        animations: list[meta.Animation] = []
        for animation_name, animation_frames in mutable_sprite.animations.items():
            frames = [
                meta.Frame(
                    x=frame.x, y=frame.y, w=frame.w, h=frame.h, duration=frame.duration
                )
                for frame in animation_frames
            ]
            animations.append(meta.Animation(name=animation_name, frames=frames))

        sprites.append(
            meta.Sprite(
                name=sprite_name,
                size=meta.Size(w=mutable_sprite.w, h=mutable_sprite.h),
                animations=animations,
            )
        )

    return meta.Data(sprites=sprites)


def main() -> None:
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-i",
        "--input",
        metavar="FILE",
        type=Path,
        required=True,
        help="path to aseprite JSON data file",
    )
    parser.add_argument(
        "-o",
        "--output",
        metavar="FILE",
        type=Path,
        required=True,
        help="path to output meta file",
    )
    args = parser.parse_args()

    data = aseprite.Data.model_validate_json(args.input.read_text())
    aseprite_data = collect_data_from_aseprite(data)
    meta_data = convert_to_meta(aseprite_data)

    with args.output.open("w", encoding="utf-8") as f:
        yaml.safe_dump(meta_data.model_dump(), f, sort_keys=False)


if __name__ == "__main__":
    main()
