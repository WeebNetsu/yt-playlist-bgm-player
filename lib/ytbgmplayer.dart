import 'package:mpv_dart/mpv_dart.dart';

int calculate() {
  return 6 * 7;
}

Future<void> testmpv() async {
  try {
    MPVPlayer mpvPlayer = MPVPlayer(
      audioOnly: true,
      debug: true,
      verbose: true,
      mpvArgs: [
        "--input-ipc-server=/tmp/my_unique_mpv.sock",
        "--script-opts=ytdl_hook-ytdl_path=/home/netsu/Downloads/yt-dlp",
      ],
    );
    await mpvPlayer.start();
    await mpvPlayer.load("https://www.youtube.com/watch?v=Fp8msa5uYsc");

    mpvPlayer.on(MPVEvents.started, null, (ev, context) {
      print("MPV STARTED PLAYING");
    });

    mpvPlayer.on(MPVEvents.status, null, (ev, context) {
      print("MPV STATUS CHANGE: ${ev.eventData}");
    });

    mpvPlayer.on(MPVEvents.timeposition, null, (ev, context) {
      print("MPV TIMEPOSITION ${ev.eventData}");
    });
  } catch (e, stackTrace) {
    print(e);
    print(stackTrace);
  }
}
