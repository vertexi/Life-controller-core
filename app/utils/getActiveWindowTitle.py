import inspect
from typing import Any, Awaitable, Callable, Union
import pyautogui

def to_coroutine(f:Callable[..., Any]):
   async def wrapper(*args, **kwargs):
      return f(*args, **kwargs)
   return wrapper

def force_awaitable(function: Union[Callable[..., Awaitable[Any]], Callable[..., Any]]) -> Callable[..., Awaitable[Any]]:
   if inspect.iscoroutinefunction(function):
      return function
   else:
      return to_coroutine(function)

def get_active_window_title():
    return pyautogui.getActiveWindowTitle()

previous_window_title = ''
async def getActiveWindowTitle():
    global previous_window_title
    windowTitle = await force_awaitable(pyautogui.getActiveWindowTitle)()
    if previous_window_title != windowTitle:
        previous_window_title = windowTitle
        return windowTitle
    return None
