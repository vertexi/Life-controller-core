import asyncio

from winrt.windows.media.control import (
    GlobalSystemMediaTransportControlsSessionManager as MediaManager,
    GlobalSystemMediaTransportControlsSessionPlaybackStatus as PlaybackStatus,
    GlobalSystemMediaTransportControlsSessionPlaybackInfo as PlaybackInfo,
)

# playbackType = {0: "Unknown", 1: "Music", 2: "Video", 3: "Image"}

# playbackStatus = {0: "Closed", 1: "Opened", 2: "Changing", 3: "Stopped", 4: "Playing", 5: "Paused"}

async def get_media_info():
    sessions = await MediaManager.request_async()
    # sessions.get_sessions()
    current_session = sessions.get_current_session()
    if current_session:  # there needs to be a media session running
        # if current_session.source_app_user_model_id == TARGET_ID:
        # print(current_session.source_app_user_model_id)
        try:
            info_dict = await current_session.try_get_media_properties_async()
            # song_attr[0] != '_' ignores system attributes
            info_dict = {song_attr: info_dict.__getattribute__(song_attr) for song_attr in dir(info_dict) if song_attr[0] != '_'}

            # converts winrt vector to list
            info_dict['genres'] = list(info_dict['genres'])
            info_dict['app_id'] = current_session.source_app_user_model_id

            del info_dict['thumbnail']
        except:
            return {}

        try:
            info = current_session.get_playback_info()
            info = {song_attr: info.__getattribute__(song_attr) for song_attr in dir(info) if song_attr[0] != '_'}
            del info['controls']
        except:
            return {}
        info_dict = {**info_dict, **info}

        return info_dict
    return {}

prev_info_dict = {}

async def getCurrentMedia():
    global prev_info_dict
    info_dict = await get_media_info()
    if prev_info_dict != info_dict:
        prev_info_dict = info_dict
    else:
        return ""
    if info_dict:
        return f"media: {info_dict['album_artist']}, title: {info_dict['title']}, app_id: {info_dict['app_id']}, playback_type: {info_dict['playback_type']}, playback_status: {info_dict['playback_status']}"
    else:
        return "media: , title: , app_id: "

if __name__ == "__main__":
    import time
    while True:
        time.sleep(1)
        res = asyncio.run(getCurrentMedia())
        if res:
            print(res)
