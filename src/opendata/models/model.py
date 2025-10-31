import pydantic
import pydantic.alias_generators


class Model(pydantic.BaseModel):
    model_config = pydantic.ConfigDict(
        alias_generator=pydantic.alias_generators.to_camel,
        extra="forbid",
        strict=True,
    )
