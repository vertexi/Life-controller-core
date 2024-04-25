# windowsInfoGatherer

dependencies:

- `pyautogui` to get active window name.
- `winrt` library to get current play media name.
- `zmq` to transfer data with other module.

## zmq library installation

### For python

In order to use zeromq over websocket, you need to install `pyzmq` with draft api supported by libzmq.

```shell
# linux based
export ZMQ_PREFIX=bundled
export ZMQ_BUILD_DRAFT=1
python3 -m pip install pyzmq --no-binary pyzmq

# windows
set ZMQ_BUILD_DRAFT=1
python3 -m pip install pyzmq --no-binary pyzmq
```

this python script to test `pyzmq` installation

```python
import zmq

print('pyzmq-%s' % zmq.__version__)
# pyzmq-26.0.2
print('libzmq-%s' % zmq.zmq_version())
# libzmq-4.3.5
print('Draft API available: %s' % zmq.DRAFT_API)
# Draft API available: True
```

### For c++

use vcpkg, use following vcpkg.json just works fine. The websockets of cppzmq is not supported by vcpkg, But I have been pull a request.
[vcpkg pull request](https://github.com/microsoft/vcpkg/pull/38408)

```json
      {
        "name": "cppzmq",
        "features": ["websockets"]
      }
```

and of course cmake file to link zmq library.

```cmake
find_package(cppzmq CONFIG REQUIRED)
target_link_libraries(main PRIVATE cppzmq)
```
