#ifndef __CONFIG_H__
#define __CONFIG_H__

#define GROUP_NAME "com.canonical.Unity.Scope.Music.Spotify"
#define UNIQUE_NAME "/com/canonical/unity/scope/music/spotify"

/* We add this as a temporary workaround. It will not be needed in future API iterations */
#define unity_object_unref(object) g_object_unref(object)

#define CATEGORY_ICON_PATH "/usr/share/icons/unity-icon-theme/places/svg/service-spotify.svg"

#endif /* __CONFIG_H__ */
