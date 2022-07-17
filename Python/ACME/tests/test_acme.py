from asyncio import StreamReader
from pathlib import Path
import pytest

from ..src.main import (
    Agent,
    export_response,
    create_command,
    create_proxy_arg,
    create_upload_arg,
    parse_response,
    get_agent_by_name,
)


def test_export_response(tmp_path: Path):
    test_file = tmp_path / "export"
    out_bytes = b"test_string"
    export_response(out_bytes, test_file)
    with open(test_file, "rb") as f:
        assert f.read() == out_bytes


def test_create_command():
    command = b"invoke\x00"
    args = b"ls\x00"
    expected = b"\x00\x00\x00\x16\x00\x00\x00\x07invoke\x00\x00\x00\x00\x03ls\x00"
    actual = create_command(command, args)
    assert expected == actual


def test_create_proxy_arg():
    local_port = "8080"
    remote_host = "foo.com"
    remote_port = "8081"
    expected = b"\x00\x00\x00\x08foo.com\x00\x00\x00\x00\x058081\x00\x1f\x90"
    actual = create_proxy_arg(local_port, remote_host, remote_port)
    assert expected == actual


def test_create_upload_arg(tmp_path: Path):
    src = tmp_path / "upload"
    with open(src, "wb") as f:
        data = b"hello world"
        f.write(data)
        f.flush()
    dst = "/tmp/evil.txt"
    expected = b"\x00\x00\x00\x0e/tmp/evil.txt\x00\x00\x00\x00\x0bhello world"
    actual = create_upload_arg(src, dst)
    assert expected == actual


@pytest.mark.asyncio
async def test_parse_response():
    reader = StreamReader()
    reader.feed_data(
        b"\x00\x00\x00\x1f\x00\x00\x00\x00\x00\x00\x00\x13roadrunner checkin\x00"
    )
    expected = (0, b"roadrunner checkin\x00")
    actual = await parse_response(reader)
    print(actual)
    assert actual == expected


def test_get_agent_by_name():
    agents: list[Agent] = []
    expected = "myagent"
    new_agent = Agent()
    new_agent.name = expected
    agents.append(new_agent)
    actual = get_agent_by_name("myagent", agents)
    assert actual.name == expected
