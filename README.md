This is an attempt to create a Spotify scope for the Unity dash.

It uses the Spotify web API to find informations about the artists
found in the Spotify catalogue and displays the first 5 results
(usually these are the most relevant).

It relies on the Gio and Glib-Json libraries for fetching the
json files containing the informations that are needed and parsing
them.

It could be that the included Makefile needs some changes to be
adapted to your system. Once these changes are made you can compile
and install with

`make` and 
`sudo make install`

Before installing the scope you can already test the generated
executable "unity-spotify-scope" by executing it in the code
folder. In case another version of the same scope is already
running you have to kill it before testing the one you've built.

The scope is activated when a search in the music scope is performed
(either in the Music or Home scopes depending on your settings)
and terminated automatically after some time of inactivity.

Enjoy!
