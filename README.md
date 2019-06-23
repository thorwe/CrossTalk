![Logo](https://github.com/thorwe/CrossTalk/raw/master/res/logo_320x60.png "CrossTalk")

CrossTalk is a Teamspeak 3 plugin that enhances the general audio experience and provides advanced features for commanders.

[pledgie]: https://www.pledgie.com/campaigns/18898

## Features

### Positional Audio
The [Positional Audio](https://github.com/thorwe/CrossTalk/wiki/Positional-Audio "Positional Audio") module enables you to hear clients from the position they are in a supported game.
![Positional Audio img](https://github.com/thorwe/CrossTalk/raw/master/misc/ct_screenie_posa.png "Positional Audio")

### Stereo Position Spread  
![Stereo Position Spread](https://github.com/thorwe/CrossTalk/raw/master/misc/ct_screenie_ps.png "Stereo Position Spread")

### ChannelMuter
You want to listen only to a few selected people in a crowded channel? Meet the ChannelMuter.
### Cross-Server Push-to-Talk & Whispering

### Deprecated: Ducking
![Ducking has its own plugin now](https://github.com/thorwe/teamspeak-plugin-ducker)

### Deprecated: Radio FX
![Radio FX has its own plugin now](https://github.com/thorwe/teamspeak-plugin-radiofx)

## Installation
It is recommended to install the plugin directly from within the client, so that it gets automatically updated. In you TeamSpeak client, go to Tools->Options->Addons->Browse, search for the "CrossTalk" plugin and install.

## Support

If you have a question, please check this README, the wiki, and the [list of
known issues][troubleshoot].

[troubleshoot]: https://github.com/thorwe/CrossTalk/wiki/Troubleshoot

If you think you found a bug in CrossTalk, you can [submit an issue](https://github.com/thorwe/CrossTalk/issues/new).

## Compiling yourself
After cloning, you'll have to manually initialize the submodules:
```
git submodule update --init --recursive
```

Qt in the minor version of the client is required, e.g.

```
mkdir build32 & pushd build32
cmake -G "Visual Studio 15 2017" -DCMAKE_PREFIX_PATH="path_to/Qt/5.6/msvc2015" ..
popd
mkdir build64 & pushd build64
cmake -G "Visual Studio 15 2017 Win64"  -DCMAKE_PREFIX_PATH="path_to/Qt/5.6/msvc2015_64" ..
```
