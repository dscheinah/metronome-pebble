# Metronome app for Pebble

This is a haptic metronome app for Pebble devices. It gives you visual and vibration feedback to display the tempo. It is also possible to configure the time signature and tap in a tempo.

# Development

The `docker-compose.yml` provides a setup for the SDK by configuring https://hub.docker.com/r/bboehmke/pebble-dev. Use it with:
`docker-compose run --rm sdk`.

If you are not able to use Docker (e.g. the emulator does not work on Windows), you can download a prebuilt VM disk for https://www.virtualbox.org/ from https://willow.systems/pebble/vm/.
