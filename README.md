# Homemade Keyboard
Why buy a keyboard when I can make one for 3 times the cost!?

# Inspiration
While surfing the net a while back I came across [The Redox Keyboard](https://github.com/mattdibi/redox-keyboard). It's an open source split keyboard design with lots of great documentation. It was perfect for me since I just recently got a 3d printer and I've never dabbled into the keyboard making scene. Plus I've always thought split keybpards looked cool.

At the end of the project it cost me a little over $300 USD and about 3 weeks of hobby time to get the keyboard complete. The bulk of the price were the Cherry MX Green switches and keycaps, each cost about $90 with shipping. As for time, the first week was spent adding a numpad to the right half of the keyboard, the second week soldering all the switches together, the last week was spent writing the software and tweaking it to my liking.

I decided to hand solder the keyboard for 2 main reasons. First, since I added the numpad I couldn't order one that was designed for the Redox (I have no idea how to desgn a PCB). Second, I'm really bad at soldering and even worse at soldering surface mount parts so I figured soldering about a million switches by hand would be easier.

I also decided to not use the [QMK](https://github.com/qmk/qmk_firmware) keyboard firmware simply because I write software for a living and writing code for a keyboard seemed like the easiest part of the project. (I know a pretty dumb reason)

# The Build
Since I wanted a numpad the first step was finding a way to attach a numpad to the Redox design. I ended up finding one on [Thingiverse](https://www.thingiverse.com/thing:3682168) to base the design off of. I ended up opening the file in Blender and ripped it apart so that I only had the holes for the switches. I then played with it some so that it looks like it would attach to the right side of the split. I don't have any progress gif/images but [here are final versions](/Keyboard/Models/NumpadTop.stl) I ended up printing. I also made some small changes to the right side of the Redox design so that the corner would fit a little better. All models printed can be found in the [Models section](https://github.com/ToasterFuel/Keyboard/tree/master/Models) of the repo.

# Thoughts

I'm actually surprised with how well this turned out. Sanding, painting, and spraying polyurethane is actually a pretty good finish for the keyboard.
Sparkfun's Pro Micro is very easy to use (Having the pin numbers printed on the board is a huge quality of life improvement)
My favorite part of the project was designing the numpad and how it should attach to the right side.

# Improvements

Don't use PLA, I've always liked how wood looks.
I will sometimes get a double key press out of the keys, I need to figure out if this is a wiring issue, software issue, or me not used to the green switches (and figure out how to fix)
Tilt, I like my keyboards with a little step in the back
