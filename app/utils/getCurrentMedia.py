import asyncio

from winrt.windows.media.control import \
    GlobalSystemMediaTransportControlsSessionManager as MediaManager


async def get_media_info():
    sessions = await MediaManager.request_async()

    # This source_app_user_model_id check and if statement is optional
    # Use it if you want to only get a certain player/program's media
    # (e.g. only chrome.exe's media not any other program's).

    # To get the ID, use a breakpoint() to run sessions.get_current_session()
    # while the media you want to get is playing.
    # Then set TARGET_ID to the string this call returns.

    current_session = sessions.get_current_session()
    if current_session:  # there needs to be a media session running
        # if current_session.source_app_user_model_id == TARGET_ID:
        # print(current_session.source_app_user_model_id)
        info = await current_session.try_get_media_properties_async()

        # song_attr[0] != '_' ignores system attributes
        info_dict = {song_attr: info.__getattribute__(song_attr) for song_attr in dir(info) if song_attr[0] != '_'}

        # converts winrt vector to list
        info_dict['genres'] = list(info_dict['genres'])
        info_dict['app_id'] = current_session.source_app_user_model_id

        return f"media: {info_dict['album_artist']}, title: {info_dict['title']}, app_id: {info_dict['app_id']}"

    return f"media: , title: , app_id: "

def getCurrentMedia():
    return asyncio.run(get_media_info())
