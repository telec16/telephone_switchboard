# Telephone switchboard

Make your own old-school automatic switchboard for private networks!

# French words

I used some French acronyms, here is the definitions (of those I found):
* RTC: [Réseau téléphonique commuté](https://fr.wikipedia.org/wiki/R%C3%A9seau_t%C3%A9l%C3%A9phonique_commut%C3%A9)
* PTT: [Poste, télégraphes et téléphones](https://fr.wikipedia.org/wiki/Postes,_t%C3%A9l%C3%A9graphes_et_t%C3%A9l%C3%A9phones_(France))

# How analog phones works

You can find a lot of info on the Internet, but to sum up, you have three main states:
* Waiting
* Ringing
* In communication (with another phone, or with the switchboard (which I call PTT))

The phone must always have 48VDC, 40mA:

![Phone waiting](https://github.com/telec16/telephone_switchboard/raw/master/schematics/standby.png "The phone on standby")

If you want to ring it, add 48VAC in series:

![Phone ringing](https://github.com/telec16/telephone_switchboard/raw/master/schematics/ring.png "The phone ringing")

And if you want to connect two phones, you can do it this way (among others). I used 12V instead of 48V because the lines are really short, and thus don't have the same voltage drop as real lines:

![Phone connected](https://github.com/telec16/telephone_switchboard/raw/master/schematics/communication.png "The two phones in communication")

# Full schematic

![schematic](https://github.com/telec16/telephone_switchboard/raw/master/schematics/full_schematic.png "An example with three phones")

The switches are relays, controled by an Arduino. There also a transistor at the input of each line, to detect the pulses and send them back to the Arduino.

# Photos

![full view](https://github.com/telec16/telephone_switchboard/raw/master/pictures/full_view.png "Full view")

![close up](https://github.com/telec16/telephone_switchboard/raw/master/pictures/closeup.png "Close up, you can see the three transformers (left), the linear regulators (top), the transistors (bottom), and the relays (right).")

![relays](https://github.com/telec16/telephone_switchboard/raw/master/pictures/relays.png "Relays connections")

# License

This work is licensed under a [Creative Commons Attribution-ShareAlike 4.0
International License][cc-by-sa].

[![CC BY-SA 4.0][cc-by-sa-image]][cc-by-sa]

[cc-by-sa]: http://creativecommons.org/licenses/by-sa/4.0/
[cc-by-sa-image]: https://licensebuttons.net/l/by-sa/4.0/88x31.png
[cc-by-sa-shield]: https://img.shields.io/badge/License-CC%20BY--SA%204.0-lightgrey.svg
