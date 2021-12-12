# All about SK6805-1515 LEDs
(Seriously, this is more than you ever wanted to know about these suckers)

These chips are made by a few different people.  
I've seen at least [Normand LED](http://www.normandled.com/upload/201903/SK6805-1515%20LED%20Datasheet.pdf), [iPixel LED](https://cdn-shop.adafruit.com/product-files/4492/Datasheet.pdf), and the various Aliexpress sources. The below information is culled from all these sources plus personal experiance.

## Chip Package and Marking
The chips themselves, as well as being TINY, seem to be marked slightly differently in each production batch. 
The data sheet illustration doesn't quite match most of the ones I've seen. See highlights below:

![](images/package1.JPG)
and
![](images/package2.JPG)

I've had boards assembled to test this **the arrow is pointing towards the VDD pin**, regardless of it the spacing.

The data sheets aren't super helpful in keeping track of the part top and bottom layers. In my KiCad work I've used the following footprint to help keep my own sanity:
![](images/1515-footprint.PNG)

This then translates to the following loading diagram for pick and place (red emphasis mine):
![](images/feeder-loading.PNG)

## LED Current Usage
This is where I've found the least information about these chips. The spec sheet helpfully says `0.1 Watt`. When you scale this to over 1,000 LEDs this has major impacts on your power supply. I decided to do some testing.

## Background About RGB and HSV
See [Adafruit's excellent guide](https://learn.adafruit.com/adafruit-neopixel-uberguide/arduino-library-use#hsv-hue-saturation-value-colors-dot-dot-dot-3024464-41) for further reading. We'll be working in both RGB (great for determining how much power the LED will use) and HSV (great for actually fading between colors in a human-readable way).

## Formula for Current Use
Ultimately, the strangest finding is that the 1515 LEDs use a non-trivial amount of current **while they're not illuminated**. Ultra-low power projects should take this into account. 

After some linear regression of the below data the best formula I've come up with is:


**I<sub>consumed</sub> = ((R+G+B)x2.61862) + 0.58108**

Where:
- `R,G and B` are RGB values of the desired color, in 8 bit integer (so, 0-255)
- `Iconsumed` is the current in mA consumed **per LED**
- `0.58108` accounts for the current usage at zero output

While there are very very slight differences between the R, G and B values of the LED, we're going assume they're close enough. 

This will give a slightly higher than expected reading for white values. The slope for white values is more accurately 2.566145 (rather than the 2.6, above). This amounts to single digit mA on a 147 LED strip.

### About Gamma Correction
It is entirely possible I coded the gamma correction incorrectly, however I was seeing very strange results. Lower ranges were behaving as they should, eventually what should have been red went to pink, then white. Very odd.  
See [Adafruit](https://learn.adafruit.com/led-tricks-gamma-correction) about gamma correction. It IS important, even if I couldn't get it to work correctly. 

## Test Methodology
This was tested using the following:
- 2x [Adafruit Ultra Skinny 1515 Strip](https://www.adafruit.com/product/4368) soldered together and slightly modified for a total of 148 LEDs (although only 147 were lit). Only powered from 1 side of the strip.  
- [Siglent SPD3303X-E Power Supply](https://siglentna.com/power-supplies/spd3303x-spd3303x-e-series-programmable-dc-power-supply/) set to 5.00V with a current limit set to 2A
- [Fluke 789 ProcessMeter](https://www.fluke.com/en-us/product/calibration-tools/ma-loop-calibrators/fluke-789) using the `mA` scale. This unit claims accuracy to `0.2% + 2`
- [Test Code I wrote](led-evaluation-code/led-eval-code-rev00.in) 
- Excel to log results by hand (yes, this was as frustrating as it sounds)

Measuring the current current going into the LED strip with the Fluke meter (wired in series) and ground truthing it against the power supply. 

### Graphs
![](images/current-use-over-intensity.PNG)

We can see that the current use of the R, G and B values is very very close to identical.  

The rapid non-linearity of the white values around 100/255 intensity is due to the traces in the strip no longer being negligible resistors. Simply; the first bunch of LEDs were drawing a ton of power and the latter ones were seeing a large voltage drop. This is why it's recommended to power these strips from both ends and possibly mid-span as well.

For our math, I have only used the white intensity up to about 100/255, to avoid the resistance problems.


### Pure Red Testing
| Swatch                                                  | HSV       | RGB     | Measured Current (mA) | Current Per LED (mA) |
|---------------------------------------------------------|-----------|---------|------------------|----------------------|
| ![image](https://via.placeholder.com/15/000000/000000?) | 0,100,0   | 0,0,0   | 86               | 0.5850               |
| ![image](https://via.placeholder.com/15/010000/010000?) | 0,100,1   | 1,0,0   | 88               | 0.6028               |
| ![image](https://via.placeholder.com/15/020000/020000?) | 0,100,2   | 2,0,0   | 91               | 0.6207               |
| ![image](https://via.placeholder.com/15/030000/030000?) | 0,100,3   | 3,0,0   | 93               | 0.6385               |
| ![image](https://via.placeholder.com/15/040000/040000?) | 0,100,4   | 4,0,0   | 96               | 0.6563               |
| ![image](https://via.placeholder.com/15/050000/050000?) | 0,100,5   | 5,0,0   | 98               | 0.6741               |
| ![image](https://via.placeholder.com/15/060000/060000?) | 0,100,6   | 6,0,0   | 101              | 0.6919               |
| ![image](https://via.placeholder.com/15/070000/070000?) | 0,100,7   | 7,0,0   | 103              | 0.7097               |
| ![image](https://via.placeholder.com/15/080000/080000?) | 0,100,8   | 8,0,0   | 106              | 0.7275               |
| ![image](https://via.placeholder.com/15/090000/090000?) | 0,100,9   | 9,0,0   | 108              | 0.7454               |
| ![image](https://via.placeholder.com/15/0A0000/0A0000?) | 0,100,10  | 10,0,0  | 111              | 0.7632               |
| ![image](https://via.placeholder.com/15/0B0000/0B0000?) | 0,100,11  | 11,0,0  | 114              | 0.7810               |
| ![image](https://via.placeholder.com/15/0C0000/0C0000?) | 0,100,12  | 12,0,0  | 116              | 0.7988               |
| ![image](https://via.placeholder.com/15/0D0000/0D0000?) | 0,100,13  | 13,0,0  | 119              | 0.8166               |
| ![image](https://via.placeholder.com/15/0E0000/0E0000?) | 0,100,14  | 14,0,0  | 121              | 0.8344               |
| ![image](https://via.placeholder.com/15/0F0000/0F0000?) | 0,100,15  | 15,0,0  | 124              | 0.8522               |
| ![image](https://via.placeholder.com/15/100000/100000?) | 0,100,16  | 16,0,0  | 126              | 0.8701               |
| ![image](https://via.placeholder.com/15/110000/110000?) | 0,100,17  | 17,0,0  | 129              | 0.8879               |
| ![image](https://via.placeholder.com/15/120000/120000?) | 0,100,18  | 18,0,0  | 131              | 0.9057               |
| ![image](https://via.placeholder.com/15/130000/130000?) | 0,100,19  | 19,0,0  | 134              | 0.9235               |
| ![image](https://via.placeholder.com/15/140000/140000?) | 0,100,20  | 20,0,0  | 137              | 0.9413               |
| ![image](https://via.placeholder.com/15/150000/150000?) | 0,100,21  | 21,0,0  | 139              | 0.9591               |
| ![image](https://via.placeholder.com/15/160000/160000?) | 0,100,22  | 22,0,0  | 141              | 0.9769               |
| ![image](https://via.placeholder.com/15/170000/170000?) | 0,100,23  | 23,0,0  | 145              | 0.9948               |
| ![image](https://via.placeholder.com/15/180000/180000?) | 0,100,24  | 24,0,0  | 147              | 1.0126               |
| ![image](https://via.placeholder.com/15/190000/190000?) | 0,100,25  | 25,0,0  | 150              | 1.0304               |
| ![image](https://via.placeholder.com/15/1A0000/1A0000?) | 0,100,26  | 26,0,0  | 152              | 1.0482               |
| ![image](https://via.placeholder.com/15/1B0000/1B0000?) | 0,100,27  | 27,0,0  | 155              | 1.0660               |
| ![image](https://via.placeholder.com/15/1C0000/1C0000?) | 0,100,28  | 28,0,0  | 157              | 1.0838               |
| ![image](https://via.placeholder.com/15/1D0000/1D0000?) | 0,100,29  | 29,0,0  | 160              | 1.1016               |
| ![image](https://via.placeholder.com/15/1E0000/1E0000?) | 0,100,30  | 30,0,0  | 162              | 1.1194               |
| ![image](https://via.placeholder.com/15/1F0000/1F0000?) | 0,100,31  | 31,0,0  | 165              | 1.1373               |
| ![image](https://via.placeholder.com/15/200000/200000?) | 0,100,32  | 32,0,0  | 168              | 1.1551               |
| ![image](https://via.placeholder.com/15/210000/210000?) | 0,100,33  | 33,0,0  | 170              | 1.1729               |
| ![image](https://via.placeholder.com/15/220000/220000?) | 0,100,34  | 34,0,0  | 173              | 1.1907               |
| ![image](https://via.placeholder.com/15/230000/230000?) | 0,100,35  | 35,0,0  | 175              | 1.2085               |
| ![image](https://via.placeholder.com/15/240000/240000?) | 0,100,36  | 36,0,0  | 178              | 1.2263               |
| ![image](https://via.placeholder.com/15/250000/250000?) | 0,100,37  | 37,0,0  | 180              | 1.2441               |
| ![image](https://via.placeholder.com/15/260000/260000?) | 0,100,38  | 38,0,0  | 182              | 1.2620               |
| ![image](https://via.placeholder.com/15/270000/270000?) | 0,100,39  | 39,0,0  | 186              | 1.2798               |
| ![image](https://via.placeholder.com/15/280000/280000?) | 0,100,40  | 40,0,0  | 188              | 1.2976               |
| ![image](https://via.placeholder.com/15/290000/290000?) | 0,100,41  | 41,0,0  | 191              | 1.3154               |
| ![image](https://via.placeholder.com/15/2A0000/2A0000?) | 0,100,42  | 42,0,0  | 193              | 1.3332               |
| ![image](https://via.placeholder.com/15/2B0000/2B0000?) | 0,100,43  | 43,0,0  | 196              | 1.3510               |
| ![image](https://via.placeholder.com/15/2C0000/2C0000?) | 0,100,44  | 44,0,0  | 198              | 1.3688               |
| ![image](https://via.placeholder.com/15/2D0000/2D0000?) | 0,100,45  | 45,0,0  | 201              | 1.3867               |
| ![image](https://via.placeholder.com/15/2E0000/2E0000?) | 0,100,46  | 46,0,0  | 203              | 1.4045               |
| ![image](https://via.placeholder.com/15/2F0000/2F0000?) | 0,100,47  | 47,0,0  | 206              | 1.4223               |
| ![image](https://via.placeholder.com/15/300000/300000?) | 0,100,48  | 48,0,0  | 209              | 1.4401               |
| ![image](https://via.placeholder.com/15/310000/310000?) | 0,100,49  | 49,0,0  | 211              | 1.4579               |
| ![image](https://via.placeholder.com/15/320000/320000?) | 0,100,50  | 50,0,0  | 214              | 1.4757               |
| ![image](https://via.placeholder.com/15/330000/330000?) | 0,100,51  | 51,0,0  | 216              | 1.4935               |
| ![image](https://via.placeholder.com/15/340000/340000?) | 0,100,52  | 52,0,0  | 219              | 1.5114               |
| ![image](https://via.placeholder.com/15/350000/350000?) | 0,100,53  | 53,0,0  | 222              | 1.5292               |
| ![image](https://via.placeholder.com/15/360000/360000?) | 0,100,54  | 54,0,0  | 224              | 1.5470               |
| ![image](https://via.placeholder.com/15/370000/370000?) | 0,100,55  | 55,0,0  | 227              | 1.5648               |
| ![image](https://via.placeholder.com/15/380000/380000?) | 0,100,56  | 56,0,0  | 229              | 1.5826               |
| ![image](https://via.placeholder.com/15/390000/390000?) | 0,100,57  | 57,0,0  | 232              | 1.6004               |
| ![image](https://via.placeholder.com/15/3A0000/3A0000?) | 0,100,58  | 58,0,0  | 234              | 1.6182               |
| ![image](https://via.placeholder.com/15/3B0000/3B0000?) | 0,100,59  | 59,0,0  | 237              | 1.6360               |
| ![image](https://via.placeholder.com/15/3C0000/3C0000?) | 0,100,60  | 60,0,0  | 239              | 1.6539               |
| ![image](https://via.placeholder.com/15/3D0000/3D0000?) | 0,100,61  | 61,0,0  | 242              | 1.6717               |
| ![image](https://via.placeholder.com/15/3E0000/3E0000?) | 0,100,62  | 62,0,0  | 245              | 1.6895               |
| ![image](https://via.placeholder.com/15/3F0000/3F0000?) | 0,100,63  | 63,0,0  | 247              | 1.7073               |
| ![image](https://via.placeholder.com/15/400000/400000?) | 0,100,64  | 64,0,0  | 250              | 1.7251               |
| ![image](https://via.placeholder.com/15/410000/410000?) | 0,100,65  | 65,0,0  | 252              | 1.7429               |
| ![image](https://via.placeholder.com/15/420000/420000?) | 0,100,66  | 66,0,0  | 255              | 1.7607               |
| ![image](https://via.placeholder.com/15/430000/430000?) | 0,100,67  | 67,0,0  | 257              | 1.7786               |
| ![image](https://via.placeholder.com/15/440000/440000?) | 0,100,68  | 68,0,0  | 260              | 1.7964               |
| ![image](https://via.placeholder.com/15/450000/450000?) | 0,100,69  | 69,0,0  | 263              | 1.8142               |
| ![image](https://via.placeholder.com/15/460000/460000?) | 0,100,70  | 70,0,0  | 265              | 1.8320               |
| ![image](https://via.placeholder.com/15/470000/470000?) | 0,100,71  | 71,0,0  | 268              | 1.8498               |
| ![image](https://via.placeholder.com/15/480000/480000?) | 0,100,72  | 72,0,0  | 270              | 1.8676               |
| ![image](https://via.placeholder.com/15/490000/490000?) | 0,100,73  | 73,0,0  | 273              | 1.8854               |
| ![image](https://via.placeholder.com/15/4A0000/4A0000?) | 0,100,74  | 74,0,0  | 275              | 1.9033               |
| ![image](https://via.placeholder.com/15/4B0000/4B0000?) | 0,100,75  | 75,0,0  | 278              | 1.9211               |
| ![image](https://via.placeholder.com/15/4C0000/4C0000?) | 0,100,76  | 76,0,0  | 280              | 1.9389               |
| ![image](https://via.placeholder.com/15/4D0000/4D0000?) | 0,100,77  | 77,0,0  | 283              | 1.9567               |
| ![image](https://via.placeholder.com/15/4E0000/4E0000?) | 0,100,78  | 78,0,0  | 286              | 1.9745               |
| ![image](https://via.placeholder.com/15/4F0000/4F0000?) | 0,100,79  | 79,0,0  | 288              | 1.9923               |
| ![image](https://via.placeholder.com/15/500000/500000?) | 0,100,80  | 80,0,0  | 291              | 2.0101               |
| ![image](https://via.placeholder.com/15/510000/510000?) | 0,100,81  | 81,0,0  | 293              | 2.0280               |
| ![image](https://via.placeholder.com/15/520000/520000?) | 0,100,82  | 82,0,0  | 296              | 2.0458               |
| ![image](https://via.placeholder.com/15/530000/530000?) | 0,100,83  | 83,0,0  | 299              | 2.0636               |
| ![image](https://via.placeholder.com/15/540000/540000?) | 0,100,84  | 84,0,0  | 301              | 2.0814               |
| ![image](https://via.placeholder.com/15/550000/550000?) | 0,100,85  | 85,0,0  | 304              | 2.0992               |
| ![image](https://via.placeholder.com/15/560000/560000?) | 0,100,86  | 86,0,0  | 306              | 2.1170               |
| ![image](https://via.placeholder.com/15/570000/570000?) | 0,100,87  | 87,0,0  | 309              | 2.1348               |
| ![image](https://via.placeholder.com/15/580000/580000?) | 0,100,88  | 88,0,0  | 311              | 2.1526               |
| ![image](https://via.placeholder.com/15/590000/590000?) | 0,100,89  | 89,0,0  | 314              | 2.1705               |
| ![image](https://via.placeholder.com/15/5A0000/5A0000?) | 0,100,90  | 90,0,0  | 316              | 2.1883               |
| ![image](https://via.placeholder.com/15/5B0000/5B0000?) | 0,100,91  | 91,0,0  | 319              | 2.2061               |
| ![image](https://via.placeholder.com/15/5C0000/5C0000?) | 0,100,92  | 92,0,0  | 321              | 2.2239               |
| ![image](https://via.placeholder.com/15/5D0000/5D0000?) | 0,100,93  | 93,0,0  | 324              | 2.2417               |
| ![image](https://via.placeholder.com/15/5E0000/5E0000?) | 0,100,94  | 94,0,0  | 327              | 2.2595               |
| ![image](https://via.placeholder.com/15/5F0000/5F0000?) | 0,100,95  | 95,0,0  | 329              | 2.2773               |
| ![image](https://via.placeholder.com/15/600000/600000?) | 0,100,96  | 96,0,0  | 332              | 2.2952               |
| ![image](https://via.placeholder.com/15/610000/610000?) | 0,100,97  | 97,0,0  | 334              | 2.3130               |
| ![image](https://via.placeholder.com/15/620000/620000?) | 0,100,98  | 98,0,0  | 337              | 2.3308               |
| ![image](https://via.placeholder.com/15/630000/630000?) | 0,100,99  | 99,0,0  | 340              | 2.3486               |
| ![image](https://via.placeholder.com/15/640000/640000?) | 0,100,100 | 100,0,0 | 342              | 2.3664               |
| ![image](https://via.placeholder.com/15/650000/650000?) | 0,100,101 | 101,0,0 | 345              | 2.3842               |
| ![image](https://via.placeholder.com/15/660000/660000?) | 0,100,102 | 102,0,0 | 347              | 2.4020               |
| ![image](https://via.placeholder.com/15/670000/670000?) | 0,100,103 | 103,0,0 | 350              | 2.4199               |
| ![image](https://via.placeholder.com/15/680000/680000?) | 0,100,104 | 104,0,0 | 352              | 2.4377               |
| ![image](https://via.placeholder.com/15/690000/690000?) | 0,100,105 | 105,0,0 | 357              | 2.4555               |
| ![image](https://via.placeholder.com/15/6A0000/6A0000?) | 0,100,106 | 106,0,0 | 360              | 2.4733               |
| ![image](https://via.placeholder.com/15/6B0000/6B0000?) | 0,100,107 | 107,0,0 | 363              | 2.4911               |
| ![image](https://via.placeholder.com/15/6C0000/6C0000?) | 0,100,108 | 108,0,0 | 365              | 2.5089               |
| ![image](https://via.placeholder.com/15/6D0000/6D0000?) | 0,100,109 | 109,0,0 | 368              | 2.5267               |
| ![image](https://via.placeholder.com/15/6E0000/6E0000?) | 0,100,110 | 110,0,0 | 370              | 2.5446               |
| ![image](https://via.placeholder.com/15/6F0000/6F0000?) | 0,100,111 | 111,0,0 | 373              | 2.5624               |
| ![image](https://via.placeholder.com/15/700000/700000?) | 0,100,112 | 112,0,0 | 375              | 2.5802               |
| ![image](https://via.placeholder.com/15/710000/710000?) | 0,100,113 | 113,0,0 | 378              | 2.5980               |
| ![image](https://via.placeholder.com/15/720000/720000?) | 0,100,114 | 114,0,0 | 380              | 2.6158               |
| ![image](https://via.placeholder.com/15/730000/730000?) | 0,100,115 | 115,0,0 | 383              | 2.6336               |
| ![image](https://via.placeholder.com/15/740000/740000?) | 0,100,116 | 116,0,0 | 386              | 2.6514               |
| ![image](https://via.placeholder.com/15/750000/750000?) | 0,100,117 | 117,0,0 | 388              | 2.6692               |
| ![image](https://via.placeholder.com/15/760000/760000?) | 0,100,118 | 118,0,0 | 391              | 2.6871               |
| ![image](https://via.placeholder.com/15/770000/770000?) | 0,100,119 | 119,0,0 | 393              | 2.7049               |
| ![image](https://via.placeholder.com/15/780000/780000?) | 0,100,120 | 120,0,0 | 396              | 2.7227               |
| ![image](https://via.placeholder.com/15/790000/790000?) | 0,100,121 | 121,0,0 | 398              | 2.7405               |
| ![image](https://via.placeholder.com/15/7A0000/7A0000?) | 0,100,122 | 122,0,0 | 400              | 2.7583               |
| ![image](https://via.placeholder.com/15/7B0000/7B0000?) | 0,100,123 | 123,0,0 | 403              | 2.7761               |
| ![image](https://via.placeholder.com/15/7C0000/7C0000?) | 0,100,124 | 124,0,0 | 406              | 2.7939               |
| ![image](https://via.placeholder.com/15/7D0000/7D0000?) | 0,100,125 | 125,0,0 | 409              | 2.8118               |
| ![image](https://via.placeholder.com/15/7E0000/7E0000?) | 0,100,126 | 126,0,0 | 411              | 2.8296               |
| ![image](https://via.placeholder.com/15/7F0000/7F0000?) | 0,100,127 | 127,0,0 | 414              | 2.8474               |
| ![image](https://via.placeholder.com/15/800000/800000?) | 0,100,128 | 128,0,0 | 417              | 2.8652               |
| ![image](https://via.placeholder.com/15/810000/810000?) | 0,100,129 | 129,0,0 | 419              | 2.8830               |
| ![image](https://via.placeholder.com/15/820000/820000?) | 0,100,130 | 130,0,0 | 422              | 2.9008               |
| ![image](https://via.placeholder.com/15/830000/830000?) | 0,100,131 | 131,0,0 | 424              | 2.9186               |
| ![image](https://via.placeholder.com/15/840000/840000?) | 0,100,132 | 132,0,0 | 427              | 2.9365               |
| ![image](https://via.placeholder.com/15/850000/850000?) | 0,100,133 | 133,0,0 | 429              | 2.9543               |
| ![image](https://via.placeholder.com/15/860000/860000?) | 0,100,134 | 134,0,0 | 432              | 2.9721               |
| ![image](https://via.placeholder.com/15/870000/870000?) | 0,100,135 | 135,0,0 | 434              | 2.9899               |
| ![image](https://via.placeholder.com/15/880000/880000?) | 0,100,136 | 136,0,0 | 437              | 3.0077               |
| ![image](https://via.placeholder.com/15/890000/890000?) | 0,100,137 | 137,0,0 | 439              | 3.0255               |
| ![image](https://via.placeholder.com/15/8A0000/8A0000?) | 0,100,138 | 138,0,0 | 440              | 3.0433               |
| ![image](https://via.placeholder.com/15/8B0000/8B0000?) | 0,100,139 | 139,0,0 | 442              | 3.0612               |
| ![image](https://via.placeholder.com/15/8C0000/8C0000?) | 0,100,140 | 140,0,0 | 445              | 3.0790               |
| ![image](https://via.placeholder.com/15/8D0000/8D0000?) | 0,100,141 | 141,0,0 | 447              | 3.0968               |
| ![image](https://via.placeholder.com/15/8E0000/8E0000?) | 0,100,142 | 142,0,0 | 450              | 3.1146               |
| ![image](https://via.placeholder.com/15/8F0000/8F0000?) | 0,100,143 | 143,0,0 | 452              | 3.1324               |
| ![image](https://via.placeholder.com/15/900000/900000?) | 0,100,144 | 144,0,0 | 455              | 3.1502               |
| ![image](https://via.placeholder.com/15/910000/910000?) | 0,100,145 | 145,0,0 | 460              | 3.1680               |
| ![image](https://via.placeholder.com/15/920000/920000?) | 0,100,146 | 146,0,0 | 463              | 3.1858               |
| ![image](https://via.placeholder.com/15/930000/930000?) | 0,100,147 | 147,0,0 | 465              | 3.2037               |
| ![image](https://via.placeholder.com/15/940000/940000?) | 0,100,148 | 148,0,0 | 468              | 3.2215               |
| ![image](https://via.placeholder.com/15/950000/950000?) | 0,100,149 | 149,0,0 | 470              | 3.2393               |
| ![image](https://via.placeholder.com/15/960000/960000?) | 0,100,150 | 150,0,0 | 473              | 3.2571               |
| ![image](https://via.placeholder.com/15/970000/970000?) | 0,100,151 | 151,0,0 | 475              | 3.2749               |
| ![image](https://via.placeholder.com/15/980000/980000?) | 0,100,152 | 152,0,0 | 478              | 3.2927               |
| ![image](https://via.placeholder.com/15/990000/990000?) | 0,100,153 | 153,0,0 | 481              | 3.3105               |
| ![image](https://via.placeholder.com/15/9A0000/9A0000?) | 0,100,154 | 154,0,0 | 483              | 3.3284               |
| ![image](https://via.placeholder.com/15/9B0000/9B0000?) | 0,100,155 | 155,0,0 | 486              | 3.3462               |
| ![image](https://via.placeholder.com/15/9C0000/9C0000?) | 0,100,156 | 156,0,0 | 488              | 3.3640               |
| ![image](https://via.placeholder.com/15/9D0000/9D0000?) | 0,100,157 | 157,0,0 | 491              | 3.3818               |
| ![image](https://via.placeholder.com/15/9E0000/9E0000?) | 0,100,158 | 158,0,0 | 493              | 3.3996               |
| ![image](https://via.placeholder.com/15/9F0000/9F0000?) | 0,100,159 | 159,0,0 | 496              | 3.4174               |
| ![image](https://via.placeholder.com/15/A00000/A00000?) | 0,100,160 | 160,0,0 | 499              | 3.4352               |
| ![image](https://via.placeholder.com/15/A10000/A10000?) | 0,100,161 | 161,0,0 | 501              | 3.4531               |
| ![image](https://via.placeholder.com/15/A20000/A20000?) | 0,100,162 | 162,0,0 | 504              | 3.4709               |
| ![image](https://via.placeholder.com/15/A30000/A30000?) | 0,100,163 | 163,0,0 | 507              | 3.4887               |
| ![image](https://via.placeholder.com/15/A40000/A40000?) | 0,100,164 | 164,0,0 | 509              | 3.5065               |
| ![image](https://via.placeholder.com/15/A50000/A50000?) | 0,100,165 | 165,0,0 | 511              | 3.5243               |
| ![image](https://via.placeholder.com/15/A60000/A60000?) | 0,100,166 | 166,0,0 | 514              | 3.5421               |
| ![image](https://via.placeholder.com/15/A70000/A70000?) | 0,100,167 | 167,0,0 | 517              | 3.5599               |
| ![image](https://via.placeholder.com/15/A80000/A80000?) | 0,100,168 | 168,0,0 | 519              | 3.5778               |
| ![image](https://via.placeholder.com/15/A90000/A90000?) | 0,100,169 | 169,0,0 | 522              | 3.5956               |
| ![image](https://via.placeholder.com/15/AA0000/AA0000?) | 0,100,170 | 170,0,0 | 524              | 3.6134               |
| ![image](https://via.placeholder.com/15/AB0000/AB0000?) | 0,100,171 | 171,0,0 | 527              | 3.6312               |
| ![image](https://via.placeholder.com/15/AC0000/AC0000?) | 0,100,172 | 172,0,0 | 530              | 3.6490               |
| ![image](https://via.placeholder.com/15/AD0000/AD0000?) | 0,100,173 | 173,0,0 | 532              | 3.6668               |
| ![image](https://via.placeholder.com/15/AE0000/AE0000?) | 0,100,174 | 174,0,0 | 535              | 3.6846               |
| ![image](https://via.placeholder.com/15/AF0000/AF0000?) | 0,100,175 | 175,0,0 | 538              | 3.7024               |
| ![image](https://via.placeholder.com/15/B00000/B00000?) | 0,100,176 | 176,0,0 | 540              | 3.7203               |
| ![image](https://via.placeholder.com/15/B10000/B10000?) | 0,100,177 | 177,0,0 | 543              | 3.7381               |
| ![image](https://via.placeholder.com/15/B20000/B20000?) | 0,100,178 | 178,0,0 | 545              | 3.7559               |
| ![image](https://via.placeholder.com/15/B30000/B30000?) | 0,100,179 | 179,0,0 | 548              | 3.7737               |
| ![image](https://via.placeholder.com/15/B40000/B40000?) | 0,100,180 | 180,0,0 | 551              | 3.7915               |
| ![image](https://via.placeholder.com/15/B50000/B50000?) | 0,100,181 | 181,0,0 | 553              | 3.8093               |
| ![image](https://via.placeholder.com/15/B60000/B60000?) | 0,100,182 | 182,0,0 | 556              | 3.8271               |
| ![image](https://via.placeholder.com/15/B70000/B70000?) | 0,100,183 | 183,0,0 | 558              | 3.8450               |
| ![image](https://via.placeholder.com/15/B80000/B80000?) | 0,100,184 | 184,0,0 | 561              | 3.8628               |
| ![image](https://via.placeholder.com/15/B90000/B90000?) | 0,100,185 | 185,0,0 | 564              | 3.8806               |
| ![image](https://via.placeholder.com/15/BA0000/BA0000?) | 0,100,186 | 186,0,0 | 566              | 3.8984               |
| ![image](https://via.placeholder.com/15/BB0000/BB0000?) | 0,100,187 | 187,0,0 | 569              | 3.9162               |
| ![image](https://via.placeholder.com/15/BC0000/BC0000?) | 0,100,188 | 188,0,0 | 572              | 3.9340               |
| ![image](https://via.placeholder.com/15/BD0000/BD0000?) | 0,100,189 | 189,0,0 | 574              | 3.9518               |
| ![image](https://via.placeholder.com/15/BE0000/BE0000?) | 0,100,190 | 190,0,0 | 577              | 3.9697               |
| ![image](https://via.placeholder.com/15/BF0000/BF0000?) | 0,100,191 | 191,0,0 | 580              | 3.9875               |
| ![image](https://via.placeholder.com/15/C00000/C00000?) | 0,100,192 | 192,0,0 | 582              | 4.0053               |
| ![image](https://via.placeholder.com/15/C10000/C10000?) | 0,100,193 | 193,0,0 | 585              | 4.0231               |
| ![image](https://via.placeholder.com/15/C20000/C20000?) | 0,100,194 | 194,0,0 | 590              | 4.0409               |
| ![image](https://via.placeholder.com/15/C30000/C30000?) | 0,100,195 | 195,0,0 | 593              | 4.0587               |
| ![image](https://via.placeholder.com/15/C40000/C40000?) | 0,100,196 | 196,0,0 | 595              | 4.0765               |
| ![image](https://via.placeholder.com/15/C50000/C50000?) | 0,100,197 | 197,0,0 | 598              | 4.0944               |
| ![image](https://via.placeholder.com/15/C60000/C60000?) | 0,100,198 | 198,0,0 | 601              | 4.1122               |
| ![image](https://via.placeholder.com/15/C70000/C70000?) | 0,100,199 | 199,0,0 | 603              | 4.1300               |
| ![image](https://via.placeholder.com/15/C80000/C80000?) | 0,100,200 | 200,0,0 | 606              | 4.1478               |
| ![image](https://via.placeholder.com/15/C90000/C90000?) | 0,100,201 | 201,0,0 | 609              | 4.1656               |
| ![image](https://via.placeholder.com/15/CA0000/CA0000?) | 0,100,202 | 202,0,0 | 611              | 4.1834               |
| ![image](https://via.placeholder.com/15/CB0000/CB0000?) | 0,100,203 | 203,0,0 | 614              | 4.2012               |
| ![image](https://via.placeholder.com/15/CC0000/CC0000?) | 0,100,204 | 204,0,0 | 617              | 4.2190               |
| ![image](https://via.placeholder.com/15/CD0000/CD0000?) | 0,100,205 | 205,0,0 | 619              | 4.2369               |
| ![image](https://via.placeholder.com/15/CE0000/CE0000?) | 0,100,206 | 206,0,0 | 622              | 4.2547               |
| ![image](https://via.placeholder.com/15/CF0000/CF0000?) | 0,100,207 | 207,0,0 | 625              | 4.2725               |
| ![image](https://via.placeholder.com/15/D00000/D00000?) | 0,100,208 | 208,0,0 | 627              | 4.2903               |
| ![image](https://via.placeholder.com/15/D10000/D10000?) | 0,100,209 | 209,0,0 | 630              | 4.3081               |
| ![image](https://via.placeholder.com/15/D20000/D20000?) | 0,100,210 | 210,0,0 | 633              | 4.3259               |
| ![image](https://via.placeholder.com/15/D30000/D30000?) | 0,100,211 | 211,0,0 | 635              | 4.3437               |
| ![image](https://via.placeholder.com/15/D40000/D40000?) | 0,100,212 | 212,0,0 | 638              | 4.3616               |
| ![image](https://via.placeholder.com/15/D50000/D50000?) | 0,100,213 | 213,0,0 | 641              | 4.3794               |
| ![image](https://via.placeholder.com/15/D60000/D60000?) | 0,100,214 | 214,0,0 | 643              | 4.3972               |
| ![image](https://via.placeholder.com/15/D70000/D70000?) | 0,100,215 | 215,0,0 | 646              | 4.4150               |
| ![image](https://via.placeholder.com/15/D80000/D80000?) | 0,100,216 | 216,0,0 | 649              | 4.4328               |
| ![image](https://via.placeholder.com/15/D90000/D90000?) | 0,100,217 | 217,0,0 | 651              | 4.4506               |
| ![image](https://via.placeholder.com/15/DA0000/DA0000?) | 0,100,218 | 218,0,0 | 654              | 4.4684               |
| ![image](https://via.placeholder.com/15/DB0000/DB0000?) | 0,100,219 | 219,0,0 | 656              | 4.4863               |
| ![image](https://via.placeholder.com/15/DC0000/DC0000?) | 0,100,220 | 220,0,0 | 659              | 4.5041               |
| ![image](https://via.placeholder.com/15/DD0000/DD0000?) | 0,100,221 | 221,0,0 | 662              | 4.5219               |
| ![image](https://via.placeholder.com/15/DE0000/DE0000?) | 0,100,222 | 222,0,0 | 664              | 4.5397               |
| ![image](https://via.placeholder.com/15/DF0000/DF0000?) | 0,100,223 | 223,0,0 | 667              | 4.5575               |
| ![image](https://via.placeholder.com/15/E00000/E00000?) | 0,100,224 | 224,0,0 | 670              | 4.5753               |
| ![image](https://via.placeholder.com/15/E10000/E10000?) | 0,100,225 | 225,0,0 | 672              | 4.5931               |
| ![image](https://via.placeholder.com/15/E20000/E20000?) | 0,100,226 | 226,0,0 | 677              | 4.6110               |
| ![image](https://via.placeholder.com/15/E30000/E30000?) | 0,100,227 | 227,0,0 | 680              | 4.6288               |
| ![image](https://via.placeholder.com/15/E40000/E40000?) | 0,100,228 | 228,0,0 | 683              | 4.6466               |
| ![image](https://via.placeholder.com/15/E50000/E50000?) | 0,100,229 | 229,0,0 | 685              | 4.6644               |
| ![image](https://via.placeholder.com/15/E60000/E60000?) | 0,100,230 | 230,0,0 | 688              | 4.6822               |
| ![image](https://via.placeholder.com/15/E70000/E70000?) | 0,100,231 | 231,0,0 | 691              | 4.7000               |
| ![image](https://via.placeholder.com/15/E80000/E80000?) | 0,100,232 | 232,0,0 | 693              | 4.7178               |
| ![image](https://via.placeholder.com/15/E90000/E90000?) | 0,100,233 | 233,0,0 | 695              | 4.7357               |
| ![image](https://via.placeholder.com/15/EA0000/EA0000?) | 0,100,234 | 234,0,0 | 699              | 4.7535               |
| ![image](https://via.placeholder.com/15/EB0000/EB0000?) | 0,100,235 | 235,0,0 | 701              | 4.7713               |
| ![image](https://via.placeholder.com/15/EC0000/EC0000?) | 0,100,236 | 236,0,0 | 704              | 4.7891               |
| ![image](https://via.placeholder.com/15/ED0000/ED0000?) | 0,100,237 | 237,0,0 | 706              | 4.8069               |
| ![image](https://via.placeholder.com/15/EE0000/EE0000?) | 0,100,238 | 238,0,0 | 709              | 4.8247               |
| ![image](https://via.placeholder.com/15/EF0000/EF0000?) | 0,100,239 | 239,0,0 | 712              | 4.8425               |
| ![image](https://via.placeholder.com/15/F00000/F00000?) | 0,100,240 | 240,0,0 | 714              | 4.8603               |
| ![image](https://via.placeholder.com/15/F10000/F10000?) | 0,100,241 | 241,0,0 | 717              | 4.8782               |
| ![image](https://via.placeholder.com/15/F20000/F20000?) | 0,100,242 | 242,0,0 | 720              | 4.8960               |
| ![image](https://via.placeholder.com/15/F30000/F30000?) | 0,100,243 | 243,0,0 | 722              | 4.9138               |
| ![image](https://via.placeholder.com/15/F40000/F40000?) | 0,100,244 | 244,0,0 | 725              | 4.9316               |
| ![image](https://via.placeholder.com/15/F50000/F50000?) | 0,100,245 | 245,0,0 | 730              | 4.9494               |
| ![image](https://via.placeholder.com/15/F60000/F60000?) | 0,100,246 | 246,0,0 | 733              | 4.9672               |
| ![image](https://via.placeholder.com/15/F70000/F70000?) | 0,100,247 | 247,0,0 | 736              | 4.9850               |
| ![image](https://via.placeholder.com/15/F80000/F80000?) | 0,100,248 | 248,0,0 | 738              | 5.0029               |
| ![image](https://via.placeholder.com/15/F90000/F90000?) | 0,100,249 | 249,0,0 | 741              | 5.0207               |
| ![image](https://via.placeholder.com/15/FA0000/FA0000?) | 0,100,250 | 250,0,0 | 743              | 5.0385               |
| ![image](https://via.placeholder.com/15/FB0000/FB0000?) | 0,100,251 | 251,0,0 | 749              | 5.0563               |
| ![image](https://via.placeholder.com/15/FC0000/FC0000?) | 0,100,252 | 252,0,0 |                  | 5.0741               |
| ![image](https://via.placeholder.com/15/FD0000/FD0000?) | 0,100,253 | 253,0,0 |                  | 5.0919               |
| ![image](https://via.placeholder.com/15/FE0000/FE0000?) | 0,100,254 | 254,0,0 |                  | 5.1097               |
| ![image](https://via.placeholder.com/15/FF0000/FF0000?) | 0,100,255 | 255,0,0 | 751              | 5.1276               |

### Pure Green Testing
| Swatch                                                  | HSV           | RGB     | Measured Current (mA) | Current Per LED (mA) |
|---------------------------------------------------------|---------------|---------|-----------------------|----------------------|
| ![image](https://via.placeholder.com/15/000000/000000?) | 21845,100,0   | 0,0,0   | 86                    | 0.5850               |
| ![image](https://via.placeholder.com/15/000100/000100?) | 21845,100,1   | 0,1,0   | 88                    | 0.5850               |
| ![image](https://via.placeholder.com/15/000200/000200?) | 21845,100,2   | 0,2,0   | 91                    | 0.5850               |
| ![image](https://via.placeholder.com/15/000300/000300?) | 21845,100,3   | 0,3,0   | 93                    | 0.5850               |
| ![image](https://via.placeholder.com/15/000400/000400?) | 21845,100,4   | 0,4,0   | 96                    | 0.5850               |
| ![image](https://via.placeholder.com/15/000500/000500?) | 21845,100,5   | 0,5,0   | 98                    | 0.5850               |
| ![image](https://via.placeholder.com/15/000600/000600?) | 21845,100,6   | 0,6,0   | 101                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000700/000700?) | 21845,100,7   | 0,7,0   | 103                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000800/000800?) | 21845,100,8   | 0,8,0   | 106                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000900/000900?) | 21845,100,9   | 0,9,0   | 109                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000A00/000A00?) | 21845,100,10  | 0,10,0  | 111                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000B00/000B00?) | 21845,100,11  | 0,11,0  | 114                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000C00/000C00?) | 21845,100,12  | 0,12,0  | 116                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000D00/000D00?) | 21845,100,13  | 0,13,0  | 119                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000E00/000E00?) | 21845,100,14  | 0,14,0  | 122                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000F00/000F00?) | 21845,100,15  | 0,15,0  | 124                   | 0.5850               |
| ![image](https://via.placeholder.com/15/001000/001000?) | 21845,100,16  | 0,16,0  | 127                   | 0.5850               |
| ![image](https://via.placeholder.com/15/001100/001100?) | 21845,100,17  | 0,17,0  | 129                   | 0.5850               |
| ![image](https://via.placeholder.com/15/001200/001200?) | 21845,100,18  | 0,18,0  | 132                   | 0.5850               |
| ![image](https://via.placeholder.com/15/001300/001300?) | 21845,100,19  | 0,19,0  | 134                   | 0.5850               |
| ![image](https://via.placeholder.com/15/001400/001400?) | 21845,100,20  | 0,20,0  | 137                   | 0.5850               |
| ![image](https://via.placeholder.com/15/001500/001500?) | 21845,100,21  | 0,21,0  | 140                   | 0.5850               |
| ![image](https://via.placeholder.com/15/001600/001600?) | 21845,100,22  | 0,22,0  | 142                   | 0.5850               |
| ![image](https://via.placeholder.com/15/001700/001700?) | 21845,100,23  | 0,23,0  | 145                   | 0.5850               |
| ![image](https://via.placeholder.com/15/001800/001800?) | 21845,100,24  | 0,24,0  | 147                   | 0.5850               |
| ![image](https://via.placeholder.com/15/001900/001900?) | 21845,100,25  | 0,25,0  | 150                   | 0.5850               |
| ![image](https://via.placeholder.com/15/001A00/001A00?) | 21845,100,26  | 0,26,0  | 152                   | 0.5850               |
| ![image](https://via.placeholder.com/15/001B00/001B00?) | 21845,100,27  | 0,27,0  | 155                   | 0.5850               |
| ![image](https://via.placeholder.com/15/001C00/001C00?) | 21845,100,28  | 0,28,0  | 158                   | 0.5850               |
| ![image](https://via.placeholder.com/15/001D00/001D00?) | 21845,100,29  | 0,29,0  | 160                   | 0.5850               |
| ![image](https://via.placeholder.com/15/001E00/001E00?) | 21845,100,30  | 0,30,0  | 163                   | 0.5850               |
| ![image](https://via.placeholder.com/15/001F00/001F00?) | 21845,100,31  | 0,31,0  | 165                   | 0.5850               |
| ![image](https://via.placeholder.com/15/002000/002000?) | 21845,100,32  | 0,32,0  | 168                   | 0.5850               |
| ![image](https://via.placeholder.com/15/002100/002100?) | 21845,100,33  | 0,33,0  | 171                   | 0.5850               |
| ![image](https://via.placeholder.com/15/002200/002200?) | 21845,100,34  | 0,34,0  | 173                   | 0.5850               |
| ![image](https://via.placeholder.com/15/002300/002300?) | 21845,100,35  | 0,35,0  | 176                   | 0.5850               |
| ![image](https://via.placeholder.com/15/002400/002400?) | 21845,100,36  | 0,36,0  | 178                   | 0.5850               |
| ![image](https://via.placeholder.com/15/002500/002500?) | 21845,100,37  | 0,37,0  | 181                   | 0.5850               |
| ![image](https://via.placeholder.com/15/002600/002600?) | 21845,100,38  | 0,38,0  | 183                   | 0.5850               |
| ![image](https://via.placeholder.com/15/002700/002700?) | 21845,100,39  | 0,39,0  | 186                   | 0.5850               |
| ![image](https://via.placeholder.com/15/002800/002800?) | 21845,100,40  | 0,40,0  | 189                   | 0.5850               |
| ![image](https://via.placeholder.com/15/002900/002900?) | 21845,100,41  | 0,41,0  | 191                   | 0.5850               |
| ![image](https://via.placeholder.com/15/002A00/002A00?) | 21845,100,42  | 0,42,0  | 194                   | 0.5850               |
| ![image](https://via.placeholder.com/15/002B00/002B00?) | 21845,100,43  | 0,43,0  | 196                   | 0.5850               |
| ![image](https://via.placeholder.com/15/002C00/002C00?) | 21845,100,44  | 0,44,0  | 199                   | 0.5850               |
| ![image](https://via.placeholder.com/15/002D00/002D00?) | 21845,100,45  | 0,45,0  | 202                   | 0.5850               |
| ![image](https://via.placeholder.com/15/002E00/002E00?) | 21845,100,46  | 0,46,0  | 204                   | 0.5850               |
| ![image](https://via.placeholder.com/15/002F00/002F00?) | 21845,100,47  | 0,47,0  | 207                   | 0.5850               |
| ![image](https://via.placeholder.com/15/003000/003000?) | 21845,100,48  | 0,48,0  | 209                   | 0.5850               |
| ![image](https://via.placeholder.com/15/003100/003100?) | 21845,100,49  | 0,49,0  | 212                   | 0.5850               |
| ![image](https://via.placeholder.com/15/003200/003200?) | 21845,100,50  | 0,50,0  | 214                   | 0.5850               |
| ![image](https://via.placeholder.com/15/003300/003300?) | 21845,100,51  | 0,51,0  | 217                   | 0.5850               |
| ![image](https://via.placeholder.com/15/003400/003400?) | 21845,100,52  | 0,52,0  | 220                   | 0.5850               |
| ![image](https://via.placeholder.com/15/003500/003500?) | 21845,100,53  | 0,53,0  | 222                   | 0.5850               |
| ![image](https://via.placeholder.com/15/003600/003600?) | 21845,100,54  | 0,54,0  | 225                   | 0.5850               |
| ![image](https://via.placeholder.com/15/003700/003700?) | 21845,100,55  | 0,55,0  | 227                   | 0.5850               |
| ![image](https://via.placeholder.com/15/003800/003800?) | 21845,100,56  | 0,56,0  | 230                   | 0.5850               |
| ![image](https://via.placeholder.com/15/003900/003900?) | 21845,100,57  | 0,57,0  | 232                   | 0.5850               |
| ![image](https://via.placeholder.com/15/003A00/003A00?) | 21845,100,58  | 0,58,0  | 235                   | 0.5850               |
| ![image](https://via.placeholder.com/15/003B00/003B00?) | 21845,100,59  | 0,59,0  | 238                   | 0.5850               |
| ![image](https://via.placeholder.com/15/003C00/003C00?) | 21845,100,60  | 0,60,0  | 240                   | 0.5850               |
| ![image](https://via.placeholder.com/15/003D00/003D00?) | 21845,100,61  | 0,61,0  | 243                   | 0.5850               |
| ![image](https://via.placeholder.com/15/003E00/003E00?) | 21845,100,62  | 0,62,0  | 245                   | 0.5850               |
| ![image](https://via.placeholder.com/15/003F00/003F00?) | 21845,100,63  | 0,63,0  | 248                   | 0.5850               |
| ![image](https://via.placeholder.com/15/004000/004000?) | 21845,100,64  | 0,64,0  | 250                   | 0.5850               |
| ![image](https://via.placeholder.com/15/004100/004100?) | 21845,100,65  | 0,65,0  | 253                   | 0.5850               |
| ![image](https://via.placeholder.com/15/004200/004200?) | 21845,100,66  | 0,66,0  | 256                   | 0.5850               |
| ![image](https://via.placeholder.com/15/004300/004300?) | 21845,100,67  | 0,67,0  | 258                   | 0.5850               |
| ![image](https://via.placeholder.com/15/004400/004400?) | 21845,100,68  | 0,68,0  | 261                   | 0.5850               |
| ![image](https://via.placeholder.com/15/004500/004500?) | 21845,100,69  | 0,69,0  | 263                   | 0.5850               |
| ![image](https://via.placeholder.com/15/004600/004600?) | 21845,100,70  | 0,70,0  | 266                   | 0.5850               |
| ![image](https://via.placeholder.com/15/004700/004700?) | 21845,100,71  | 0,71,0  | 268                   | 0.5850               |
| ![image](https://via.placeholder.com/15/004800/004800?) | 21845,100,72  | 0,72,0  | 271                   | 0.5850               |
| ![image](https://via.placeholder.com/15/004900/004900?) | 21845,100,73  | 0,73,0  | 274                   | 0.5850               |
| ![image](https://via.placeholder.com/15/004A00/004A00?) | 21845,100,74  | 0,74,0  | 276                   | 0.5850               |
| ![image](https://via.placeholder.com/15/004B00/004B00?) | 21845,100,75  | 0,75,0  | 279                   | 0.5850               |
| ![image](https://via.placeholder.com/15/004C00/004C00?) | 21845,100,76  | 0,76,0  | 280                   | 0.5850               |
| ![image](https://via.placeholder.com/15/004D00/004D00?) | 21845,100,77  | 0,77,0  | 284                   | 0.5850               |
| ![image](https://via.placeholder.com/15/004E00/004E00?) | 21845,100,78  | 0,78,0  | 286                   | 0.5850               |
| ![image](https://via.placeholder.com/15/004F00/004F00?) | 21845,100,79  | 0,79,0  | 289                   | 0.5850               |
| ![image](https://via.placeholder.com/15/005000/005000?) | 21845,100,80  | 0,80,0  | 292                   | 0.5850               |
| ![image](https://via.placeholder.com/15/005100/005100?) | 21845,100,81  | 0,81,0  | 294                   | 0.5850               |
| ![image](https://via.placeholder.com/15/005200/005200?) | 21845,100,82  | 0,82,0  | 297                   | 0.5850               |
| ![image](https://via.placeholder.com/15/005300/005300?) | 21845,100,83  | 0,83,0  | 299                   | 0.5850               |
| ![image](https://via.placeholder.com/15/005400/005400?) | 21845,100,84  | 0,84,0  | 302                   | 0.5850               |
| ![image](https://via.placeholder.com/15/005500/005500?) | 21845,100,85  | 0,85,0  | 305                   | 0.5850               |
| ![image](https://via.placeholder.com/15/005600/005600?) | 21845,100,86  | 0,86,0  | 307                   | 0.5850               |
| ![image](https://via.placeholder.com/15/005700/005700?) | 21845,100,87  | 0,87,0  | 310                   | 0.5850               |
| ![image](https://via.placeholder.com/15/005800/005800?) | 21845,100,88  | 0,88,0  | 312                   | 0.5850               |
| ![image](https://via.placeholder.com/15/005900/005900?) | 21845,100,89  | 0,89,0  | 315                   | 0.5850               |
| ![image](https://via.placeholder.com/15/005A00/005A00?) | 21845,100,90  | 0,90,0  | 317                   | 0.5850               |
| ![image](https://via.placeholder.com/15/005B00/005B00?) | 21845,100,91  | 0,91,0  | 320                   | 0.5850               |
| ![image](https://via.placeholder.com/15/005C00/005C00?) | 21845,100,92  | 0,92,0  | 322                   | 0.5850               |
| ![image](https://via.placeholder.com/15/005D00/005D00?) | 21845,100,93  | 0,93,0  | 325                   | 0.5850               |
| ![image](https://via.placeholder.com/15/005E00/005E00?) | 21845,100,94  | 0,94,0  | 328                   | 0.5850               |
| ![image](https://via.placeholder.com/15/005F00/005F00?) | 21845,100,95  | 0,95,0  | 330                   | 0.5850               |
| ![image](https://via.placeholder.com/15/006000/006000?) | 21845,100,96  | 0,96,0  | 333                   | 0.5850               |
| ![image](https://via.placeholder.com/15/006100/006100?) | 21845,100,97  | 0,97,0  | 335                   | 0.5850               |
| ![image](https://via.placeholder.com/15/006200/006200?) | 21845,100,98  | 0,98,0  | 338                   | 0.5850               |
| ![image](https://via.placeholder.com/15/006300/006300?) | 21845,100,99  | 0,99,0  | 340                   | 0.5850               |
| ![image](https://via.placeholder.com/15/006400/006400?) | 21845,100,100 | 0,100,0 | 343                   | 0.5850               |
| ![image](https://via.placeholder.com/15/006500/006500?) | 21845,100,101 | 0,101,0 | 346                   | 0.5850               |
| ![image](https://via.placeholder.com/15/006600/006600?) | 21845,100,102 | 0,102,0 | 348                   | 0.5850               |
| ![image](https://via.placeholder.com/15/006700/006700?) | 21845,100,103 | 0,103,0 | 351                   | 0.5850               |
| ![image](https://via.placeholder.com/15/006800/006800?) | 21845,100,104 | 0,104,0 | 354                   | 0.5850               |
| ![image](https://via.placeholder.com/15/006900/006900?) | 21845,100,105 | 0,105,0 | 356                   | 0.5850               |
| ![image](https://via.placeholder.com/15/006A00/006A00?) | 21845,100,106 | 0,106,0 | 359                   | 0.5850               |
| ![image](https://via.placeholder.com/15/006B00/006B00?) | 21845,100,107 | 0,107,0 | 360                   | 0.5850               |
| ![image](https://via.placeholder.com/15/006C00/006C00?) | 21845,100,108 | 0,108,0 | 364                   | 0.5850               |
| ![image](https://via.placeholder.com/15/006D00/006D00?) | 21845,100,109 | 0,109,0 | 366                   | 0.5850               |
| ![image](https://via.placeholder.com/15/006E00/006E00?) | 21845,100,110 | 0,110,0 | 369                   | 0.5850               |
| ![image](https://via.placeholder.com/15/006F00/006F00?) | 21845,100,111 | 0,111,0 | 371                   | 0.5850               |
| ![image](https://via.placeholder.com/15/007000/007000?) | 21845,100,112 | 0,112,0 | 374                   | 0.5850               |
| ![image](https://via.placeholder.com/15/007100/007100?) | 21845,100,113 | 0,113,0 | 377                   | 0.5850               |
| ![image](https://via.placeholder.com/15/007200/007200?) | 21845,100,114 | 0,114,0 | 379                   | 0.5850               |
| ![image](https://via.placeholder.com/15/007300/007300?) | 21845,100,115 | 0,115,0 | 382                   | 0.5850               |
| ![image](https://via.placeholder.com/15/007400/007400?) | 21845,100,116 | 0,116,0 | 384                   | 0.5850               |
| ![image](https://via.placeholder.com/15/007500/007500?) | 21845,100,117 | 0,117,0 | 387                   | 0.5850               |
| ![image](https://via.placeholder.com/15/007600/007600?) | 21845,100,118 | 0,118,0 | 390                   | 0.5850               |
| ![image](https://via.placeholder.com/15/007700/007700?) | 21845,100,119 | 0,119,0 | 392                   | 0.5850               |
| ![image](https://via.placeholder.com/15/007800/007800?) | 21845,100,120 | 0,120,0 | 395                   | 0.5850               |
| ![image](https://via.placeholder.com/15/007900/007900?) | 21845,100,121 | 0,121,0 | 397                   | 0.5850               |
| ![image](https://via.placeholder.com/15/007A00/007A00?) | 21845,100,122 | 0,122,0 | 400                   | 0.5850               |
| ![image](https://via.placeholder.com/15/007B00/007B00?) | 21845,100,123 | 0,123,0 | 402                   | 0.5850               |
| ![image](https://via.placeholder.com/15/007C00/007C00?) | 21845,100,124 | 0,124,0 | 405                   | 0.5850               |
| ![image](https://via.placeholder.com/15/007D00/007D00?) | 21845,100,125 | 0,125,0 | 407                   | 0.5850               |
| ![image](https://via.placeholder.com/15/007E00/007E00?) | 21845,100,126 | 0,126,0 | 410                   | 0.5850               |
| ![image](https://via.placeholder.com/15/007F00/007F00?) | 21845,100,127 | 0,127,0 | 413                   | 0.5850               |
| ![image](https://via.placeholder.com/15/008000/008000?) | 21845,100,128 | 0,128,0 | 415                   | 0.5850               |
| ![image](https://via.placeholder.com/15/008100/008100?) | 21845,100,129 | 0,129,0 | 418                   | 0.5850               |
| ![image](https://via.placeholder.com/15/008200/008200?) | 21845,100,130 | 0,130,0 | 420                   | 0.5850               |
| ![image](https://via.placeholder.com/15/008300/008300?) | 21845,100,131 | 0,131,0 | 423                   | 0.5850               |
| ![image](https://via.placeholder.com/15/008400/008400?) | 21845,100,132 | 0,132,0 | 425                   | 0.5850               |
| ![image](https://via.placeholder.com/15/008500/008500?) | 21845,100,133 | 0,133,0 | 428                   | 0.5850               |
| ![image](https://via.placeholder.com/15/008600/008600?) | 21845,100,134 | 0,134,0 | 431                   | 0.5850               |
| ![image](https://via.placeholder.com/15/008700/008700?) | 21845,100,135 | 0,135,0 | 433                   | 0.5850               |
| ![image](https://via.placeholder.com/15/008800/008800?) | 21845,100,136 | 0,136,0 | 436                   | 0.5850               |
| ![image](https://via.placeholder.com/15/008900/008900?) | 21845,100,137 | 0,137,0 | 438                   | 0.5850               |
| ![image](https://via.placeholder.com/15/008A00/008A00?) | 21845,100,138 | 0,138,0 | 441                   | 0.5850               |
| ![image](https://via.placeholder.com/15/008B00/008B00?) | 21845,100,139 | 0,139,0 | 443                   | 0.5850               |
| ![image](https://via.placeholder.com/15/008C00/008C00?) | 21845,100,140 | 0,140,0 | 446                   | 0.5850               |
| ![image](https://via.placeholder.com/15/008D00/008D00?) | 21845,100,141 | 0,141,0 | 449                   | 0.5850               |
| ![image](https://via.placeholder.com/15/008E00/008E00?) | 21845,100,142 | 0,142,0 | 451                   | 0.5850               |
| ![image](https://via.placeholder.com/15/008F00/008F00?) | 21845,100,143 | 0,143,0 | 454                   | 0.5850               |
| ![image](https://via.placeholder.com/15/009000/009000?) | 21845,100,144 | 0,144,0 | 456                   | 0.5850               |
| ![image](https://via.placeholder.com/15/009100/009100?) | 21845,100,145 | 0,145,0 | 459                   | 0.5850               |
| ![image](https://via.placeholder.com/15/009200/009200?) | 21845,100,146 | 0,146,0 | 461                   | 0.5850               |
| ![image](https://via.placeholder.com/15/009300/009300?) | 21845,100,147 | 0,147,0 | 464                   | 0.5850               |
| ![image](https://via.placeholder.com/15/009400/009400?) | 21845,100,148 | 0,148,0 | 467                   | 0.5850               |
| ![image](https://via.placeholder.com/15/009500/009500?) | 21845,100,149 | 0,149,0 | 469                   | 0.5850               |
| ![image](https://via.placeholder.com/15/009600/009600?) | 21845,100,150 | 0,150,0 | 472                   | 0.5850               |
| ![image](https://via.placeholder.com/15/009700/009700?) | 21845,100,151 | 0,151,0 | 474                   | 0.5850               |
| ![image](https://via.placeholder.com/15/009800/009800?) | 21845,100,152 | 0,152,0 | 477                   | 0.5850               |
| ![image](https://via.placeholder.com/15/009900/009900?) | 21845,100,153 | 0,153,0 | 480                   | 0.5850               |
| ![image](https://via.placeholder.com/15/009A00/009A00?) | 21845,100,154 | 0,154,0 | 482                   | 0.5850               |
| ![image](https://via.placeholder.com/15/009B00/009B00?) | 21845,100,155 | 0,155,0 | 485                   | 0.5850               |
| ![image](https://via.placeholder.com/15/009C00/009C00?) | 21845,100,156 | 0,156,0 | 487                   | 0.5850               |
| ![image](https://via.placeholder.com/15/009D00/009D00?) | 21845,100,157 | 0,157,0 | 490                   | 0.5850               |
| ![image](https://via.placeholder.com/15/009E00/009E00?) | 21845,100,158 | 0,158,0 | 492                   | 0.5850               |
| ![image](https://via.placeholder.com/15/009F00/009F00?) | 21845,100,159 | 0,159,0 | 495                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00A000/00A000?) | 21845,100,160 | 0,160,0 | 498                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00A100/00A100?) | 21845,100,161 | 0,161,0 | 500                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00A200/00A200?) | 21845,100,162 | 0,162,0 | 503                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00A300/00A300?) | 21845,100,163 | 0,163,0 | 505                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00A400/00A400?) | 21845,100,164 | 0,164,0 | 508                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00A500/00A500?) | 21845,100,165 | 0,165,0 | 510                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00A600/00A600?) | 21845,100,166 | 0,166,0 | 513                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00A700/00A700?) | 21845,100,167 | 0,167,0 | 516                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00A800/00A800?) | 21845,100,168 | 0,168,0 | 518                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00A900/00A900?) | 21845,100,169 | 0,169,0 | 521                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00AA00/00AA00?) | 21845,100,170 | 0,170,0 | 523                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00AB00/00AB00?) | 21845,100,171 | 0,171,0 | 526                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00AC00/00AC00?) | 21845,100,172 | 0,172,0 | 528                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00AD00/00AD00?) | 21845,100,173 | 0,173,0 | 531                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00AE00/00AE00?) | 21845,100,174 | 0,174,0 | 534                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00AF00/00AF00?) | 21845,100,175 | 0,175,0 | 536                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00B000/00B000?) | 21845,100,176 | 0,176,0 | 539                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00B100/00B100?) | 21845,100,177 | 0,177,0 | 542                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00B200/00B200?) | 21845,100,178 | 0,178,0 | 544                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00B300/00B300?) | 21845,100,179 | 0,179,0 | 547                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00B400/00B400?) | 21845,100,180 | 0,180,0 | 549                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00B500/00B500?) | 21845,100,181 | 0,181,0 | 552                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00B600/00B600?) | 21845,100,182 | 0,182,0 | 555                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00B700/00B700?) | 21845,100,183 | 0,183,0 | 557                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00B800/00B800?) | 21845,100,184 | 0,184,0 | 560                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00B900/00B900?) | 21845,100,185 | 0,185,0 | 563                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00BA00/00BA00?) | 21845,100,186 | 0,186,0 | 565                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00BB00/00BB00?) | 21845,100,187 | 0,187,0 | 568                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00BC00/00BC00?) | 21845,100,188 | 0,188,0 | 570                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00BD00/00BD00?) | 21845,100,189 | 0,189,0 | 573                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00BE00/00BE00?) | 21845,100,190 | 0,190,0 | 576                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00BF00/00BF00?) | 21845,100,191 | 0,191,0 | 578                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00C000/00C000?) | 21845,100,192 | 0,192,0 | 581                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00C100/00C100?) | 21845,100,193 | 0,193,0 | 584                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00C200/00C200?) | 21845,100,194 | 0,194,0 | 586                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00C300/00C300?) | 21845,100,195 | 0,195,0 | 589                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00C400/00C400?) | 21845,100,196 | 0,196,0 | 591                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00C500/00C500?) | 21845,100,197 | 0,197,0 | 594                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00C600/00C600?) | 21845,100,198 | 0,198,0 | 597                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00C700/00C700?) | 21845,100,199 | 0,199,0 | 600                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00C800/00C800?) | 21845,100,200 | 0,200,0 | 602                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00C900/00C900?) | 21845,100,201 | 0,201,0 | 605                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00CA00/00CA00?) | 21845,100,202 | 0,202,0 | 607                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00CB00/00CB00?) | 21845,100,203 | 0,203,0 | 610                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00CC00/00CC00?) | 21845,100,204 | 0,204,0 | 613                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00CD00/00CD00?) | 21845,100,205 | 0,205,0 | 615                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00CE00/00CE00?) | 21845,100,206 | 0,206,0 | 618                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00CF00/00CF00?) | 21845,100,207 | 0,207,0 | 621                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00D000/00D000?) | 21845,100,208 | 0,208,0 | 623                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00D100/00D100?) | 21845,100,209 | 0,209,0 | 626                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00D200/00D200?) | 21845,100,210 | 0,210,0 | 629                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00D300/00D300?) | 21845,100,211 | 0,211,0 | 631                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00D400/00D400?) | 21845,100,212 | 0,212,0 | 634                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00D500/00D500?) | 21845,100,213 | 0,213,0 | 637                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00D600/00D600?) | 21845,100,214 | 0,214,0 | 639                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00D700/00D700?) | 21845,100,215 | 0,215,0 | 642                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00D800/00D800?) | 21845,100,216 | 0,216,0 | 645                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00D900/00D900?) | 21845,100,217 | 0,217,0 | 647                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00DA00/00DA00?) | 21845,100,218 | 0,218,0 | 650                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00DB00/00DB00?) | 21845,100,219 | 0,219,0 | 653                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00DC00/00DC00?) | 21845,100,220 | 0,220,0 | 655                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00DD00/00DD00?) | 21845,100,221 | 0,221,0 | 658                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00DE00/00DE00?) | 21845,100,222 | 0,222,0 | 661                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00DF00/00DF00?) | 21845,100,223 | 0,223,0 | 663                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00E000/00E000?) | 21845,100,224 | 0,224,0 | 666                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00E100/00E100?) | 21845,100,225 | 0,225,0 | 669                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00E200/00E200?) | 21845,100,226 | 0,226,0 | 671                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00E300/00E300?) | 21845,100,227 | 0,227,0 | 674                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00E400/00E400?) | 21845,100,228 | 0,228,0 | 677                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00E500/00E500?) | 21845,100,229 | 0,229,0 | 679                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00E600/00E600?) | 21845,100,230 | 0,230,0 | 682                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00E700/00E700?) | 21845,100,231 | 0,231,0 | 685                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00E800/00E800?) | 21845,100,232 | 0,232,0 | 687                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00E900/00E900?) | 21845,100,233 | 0,233,0 | 690                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00EA00/00EA00?) | 21845,100,234 | 0,234,0 | 693                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00EB00/00EB00?) | 21845,100,235 | 0,235,0 | 695                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00EC00/00EC00?) | 21845,100,236 | 0,236,0 | 698                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00ED00/00ED00?) | 21845,100,237 | 0,237,0 | 700                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00EE00/00EE00?) | 21845,100,238 | 0,238,0 | 703                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00EF00/00EF00?) | 21845,100,239 | 0,239,0 | 706                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00F000/00F000?) | 21845,100,240 | 0,240,0 | 708                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00F100/00F100?) | 21845,100,241 | 0,241,0 | 711                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00F200/00F200?) | 21845,100,242 | 0,242,0 | 714                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00F300/00F300?) | 21845,100,243 | 0,243,0 | 716                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00F400/00F400?) | 21845,100,244 | 0,244,0 | 719                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00F500/00F500?) | 21845,100,245 | 0,245,0 | 722                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00F600/00F600?) | 21845,100,246 | 0,246,0 | 724                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00F700/00F700?) | 21845,100,247 | 0,247,0 | 727                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00F800/00F800?) | 21845,100,248 | 0,248,0 | 729                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00F900/00F900?) | 21845,100,249 | 0,249,0 | 732                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00FA00/00FA00?) | 21845,100,250 | 0,250,0 | 735                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00FB00/00FB00?) | 21845,100,251 | 0,251,0 | 737                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00FC00/00FC00?) | 21845,100,252 | 0,252,0 | 740                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00FD00/00FD00?) | 21845,100,253 | 0,253,0 | 743                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00FE00/00FE00?) | 21845,100,254 | 0,254,0 | 745                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00FF00/00FF00?) | 21845,100,255 | 0,255,0 | 751                   | 0.5850               |


### Pure Blue Testing
| Swatch                                                  | HSV           | RGB     | Measured Current (mA) | Current Per LED (mA) |
|---------------------------------------------------------|---------------|---------|-----------------------|----------------------|
| ![image](https://via.placeholder.com/15/000000/000000?) | 21845,100,0   | 0,0,0   | 86                    | 0.5850               |
| ![image](https://via.placeholder.com/15/000001/000001?) | 21845,100,1   | 0,0,1   | 88                    | 0.5850               |
| ![image](https://via.placeholder.com/15/000002/000002?) | 21845,100,2   | 0,0,2   | 91                    | 0.5850               |
| ![image](https://via.placeholder.com/15/000003/000003?) | 21845,100,3   | 0,0,3   | 93                    | 0.5850               |
| ![image](https://via.placeholder.com/15/000004/000004?) | 21845,100,4   | 0,0,4   | 96                    | 0.5850               |
| ![image](https://via.placeholder.com/15/000005/000005?) | 21845,100,5   | 0,0,5   | 98                    | 0.5850               |
| ![image](https://via.placeholder.com/15/000006/000006?) | 21845,100,6   | 0,0,6   | 101                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000007/000007?) | 21845,100,7   | 0,0,7   | 103                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000008/000008?) | 21845,100,8   | 0,0,8   | 106                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000009/000009?) | 21845,100,9   | 0,0,9   | 109                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00000A/00000A?) | 21845,100,10  | 0,0,10  | 111                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00000B/00000B?) | 21845,100,11  | 0,0,11  | 114                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00000C/00000C?) | 21845,100,12  | 0,0,12  | 116                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00000D/00000D?) | 21845,100,13  | 0,0,13  | 119                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00000E/00000E?) | 21845,100,14  | 0,0,14  | 122                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00000F/00000F?) | 21845,100,15  | 0,0,15  | 124                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000010/000010?) | 21845,100,16  | 0,0,16  | 127                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000011/000011?) | 21845,100,17  | 0,0,17  | 129                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000012/000012?) | 21845,100,18  | 0,0,18  | 132                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000013/000013?) | 21845,100,19  | 0,0,19  | 134                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000014/000014?) | 21845,100,20  | 0,0,20  | 137                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000015/000015?) | 21845,100,21  | 0,0,21  | 139                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000016/000016?) | 21845,100,22  | 0,0,22  | 142                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000017/000017?) | 21845,100,23  | 0,0,23  | 145                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000018/000018?) | 21845,100,24  | 0,0,24  | 147                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000019/000019?) | 21845,100,25  | 0,0,25  | 150                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00001A/00001A?) | 21845,100,26  | 0,0,26  | 152                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00001B/00001B?) | 21845,100,27  | 0,0,27  | 155                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00001C/00001C?) | 21845,100,28  | 0,0,28  | 157                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00001D/00001D?) | 21845,100,29  | 0,0,29  | 160                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00001E/00001E?) | 21845,100,30  | 0,0,30  | 163                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00001F/00001F?) | 21845,100,31  | 0,0,31  | 165                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000020/000020?) | 21845,100,32  | 0,0,32  | 168                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000021/000021?) | 21845,100,33  | 0,0,33  | 170                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000022/000022?) | 21845,100,34  | 0,0,34  | 173                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000023/000023?) | 21845,100,35  | 0,0,35  | 175                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000024/000024?) | 21845,100,36  | 0,0,36  | 178                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000025/000025?) | 21845,100,37  | 0,0,37  | 180                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000026/000026?) | 21845,100,38  | 0,0,38  | 183                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000027/000027?) | 21845,100,39  | 0,0,39  | 186                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000028/000028?) | 21845,100,40  | 0,0,40  | 188                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000029/000029?) | 21845,100,41  | 0,0,41  | 191                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00002A/00002A?) | 21845,100,42  | 0,0,42  | 193                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00002B/00002B?) | 21845,100,43  | 0,0,43  | 196                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00002C/00002C?) | 21845,100,44  | 0,0,44  | 198                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00002D/00002D?) | 21845,100,45  | 0,0,45  | 201                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00002E/00002E?) | 21845,100,46  | 0,0,46  | 204                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00002F/00002F?) | 21845,100,47  | 0,0,47  | 206                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000030/000030?) | 21845,100,48  | 0,0,48  | 209                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000031/000031?) | 21845,100,49  | 0,0,49  | 211                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000032/000032?) | 21845,100,50  | 0,0,50  | 214                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000033/000033?) | 21845,100,51  | 0,0,51  | 216                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000034/000034?) | 21845,100,52  | 0,0,52  | 219                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000035/000035?) | 21845,100,53  | 0,0,53  | 222                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000036/000036?) | 21845,100,54  | 0,0,54  | 224                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000037/000037?) | 21845,100,55  | 0,0,55  | 227                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000038/000038?) | 21845,100,56  | 0,0,56  | 229                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000039/000039?) | 21845,100,57  | 0,0,57  | 232                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00003A/00003A?) | 21845,100,58  | 0,0,58  | 234                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00003B/00003B?) | 21845,100,59  | 0,0,59  | 237                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00003C/00003C?) | 21845,100,60  | 0,0,60  | 239                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00003D/00003D?) | 21845,100,61  | 0,0,61  | 242                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00003E/00003E?) | 21845,100,62  | 0,0,62  | 245                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00003F/00003F?) | 21845,100,63  | 0,0,63  | 247                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000040/000040?) | 21845,100,64  | 0,0,64  | 250                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000041/000041?) | 21845,100,65  | 0,0,65  | 252                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000042/000042?) | 21845,100,66  | 0,0,66  | 255                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000043/000043?) | 21845,100,67  | 0,0,67  | 257                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000044/000044?) | 21845,100,68  | 0,0,68  | 260                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000045/000045?) | 21845,100,69  | 0,0,69  | 263                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000046/000046?) | 21845,100,70  | 0,0,70  | 265                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000047/000047?) | 21845,100,71  | 0,0,71  | 268                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000048/000048?) | 21845,100,72  | 0,0,72  | 270                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000049/000049?) | 21845,100,73  | 0,0,73  | 273                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00004A/00004A?) | 21845,100,74  | 0,0,74  | 275                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00004B/00004B?) | 21845,100,75  | 0,0,75  | 278                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00004C/00004C?) | 21845,100,76  | 0,0,76  | 280                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00004D/00004D?) | 21845,100,77  | 0,0,77  | 283                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00004E/00004E?) | 21845,100,78  | 0,0,78  | 286                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00004F/00004F?) | 21845,100,79  | 0,0,79  | 288                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000050/000050?) | 21845,100,80  | 0,0,80  | 291                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000051/000051?) | 21845,100,81  | 0,0,81  | 293                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000052/000052?) | 21845,100,82  | 0,0,82  | 296                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000053/000053?) | 21845,100,83  | 0,0,83  | 299                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000054/000054?) | 21845,100,84  | 0,0,84  | 301                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000055/000055?) | 21845,100,85  | 0,0,85  | 304                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000056/000056?) | 21845,100,86  | 0,0,86  | 306                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000057/000057?) | 21845,100,87  | 0,0,87  | 309                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000058/000058?) | 21845,100,88  | 0,0,88  | 311                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000059/000059?) | 21845,100,89  | 0,0,89  | 314                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00005A/00005A?) | 21845,100,90  | 0,0,90  | 316                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00005B/00005B?) | 21845,100,91  | 0,0,91  | 320                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00005C/00005C?) | 21845,100,92  | 0,0,92  | 321                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00005D/00005D?) | 21845,100,93  | 0,0,93  | 324                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00005E/00005E?) | 21845,100,94  | 0,0,94  | 326                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00005F/00005F?) | 21845,100,95  | 0,0,95  | 329                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000060/000060?) | 21845,100,96  | 0,0,96  | 332                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000061/000061?) | 21845,100,97  | 0,0,97  | 334                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000062/000062?) | 21845,100,98  | 0,0,98  | 337                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000063/000063?) | 21845,100,99  | 0,0,99  | 339                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000064/000064?) | 21845,100,100 | 0,0,100 | 342                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000065/000065?) | 21845,100,101 | 0,0,101 | 344                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000066/000066?) | 21845,100,102 | 0,0,102 | 347                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000067/000067?) | 21845,100,103 | 0,0,103 | 350                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000068/000068?) | 21845,100,104 | 0,0,104 | 352                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000069/000069?) | 21845,100,105 | 0,0,105 | 355                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00006A/00006A?) | 21845,100,106 | 0,0,106 | 357                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00006B/00006B?) | 21845,100,107 | 0,0,107 | 360                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00006C/00006C?) | 21845,100,108 | 0,0,108 | 362                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00006D/00006D?) | 21845,100,109 | 0,0,109 | 365                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00006E/00006E?) | 21845,100,110 | 0,0,110 | 368                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00006F/00006F?) | 21845,100,111 | 0,0,111 | 370                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000070/000070?) | 21845,100,112 | 0,0,112 | 372                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000071/000071?) | 21845,100,113 | 0,0,113 | 375                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000072/000072?) | 21845,100,114 | 0,0,114 | 378                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000073/000073?) | 21845,100,115 | 0,0,115 | 380                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000074/000074?) | 21845,100,116 | 0,0,116 | 383                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000075/000075?) | 21845,100,117 | 0,0,117 | 385                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000076/000076?) | 21845,100,118 | 0,0,118 | 388                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000077/000077?) | 21845,100,119 | 0,0,119 | 390                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000078/000078?) | 21845,100,120 | 0,0,120 | 393                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000079/000079?) | 21845,100,121 | 0,0,121 | 396                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00007A/00007A?) | 21845,100,122 | 0,0,122 | 398                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00007B/00007B?) | 21845,100,123 | 0,0,123 | 401                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00007C/00007C?) | 21845,100,124 | 0,0,124 | 403                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00007D/00007D?) | 21845,100,125 | 0,0,125 | 406                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00007E/00007E?) | 21845,100,126 | 0,0,126 | 409                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00007F/00007F?) | 21845,100,127 | 0,0,127 | 411                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000080/000080?) | 21845,100,128 | 0,0,128 | 414                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000081/000081?) | 21845,100,129 | 0,0,129 | 416                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000082/000082?) | 21845,100,130 | 0,0,130 | 419                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000083/000083?) | 21845,100,131 | 0,0,131 | 421                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000084/000084?) | 21845,100,132 | 0,0,132 | 424                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000085/000085?) | 21845,100,133 | 0,0,133 | 426                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000086/000086?) | 21845,100,134 | 0,0,134 | 429                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000087/000087?) | 21845,100,135 | 0,0,135 | 432                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000088/000088?) | 21845,100,136 | 0,0,136 | 434                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000089/000089?) | 21845,100,137 | 0,0,137 | 437                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00008A/00008A?) | 21845,100,138 | 0,0,138 | 439                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00008B/00008B?) | 21845,100,139 | 0,0,139 | 442                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00008C/00008C?) | 21845,100,140 | 0,0,140 | 444                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00008D/00008D?) | 21845,100,141 | 0,0,141 | 447                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00008E/00008E?) | 21845,100,142 | 0,0,142 | 449                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00008F/00008F?) | 21845,100,143 | 0,0,143 | 452                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000090/000090?) | 21845,100,144 | 0,0,144 | 454                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000091/000091?) | 21845,100,145 | 0,0,145 | 457                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000092/000092?) | 21845,100,146 | 0,0,146 | 460                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000093/000093?) | 21845,100,147 | 0,0,147 | 462                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000094/000094?) | 21845,100,148 | 0,0,148 | 465                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000095/000095?) | 21845,100,149 | 0,0,149 | 467                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000096/000096?) | 21845,100,150 | 0,0,150 | 470                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000097/000097?) | 21845,100,151 | 0,0,151 | 472                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000098/000098?) | 21845,100,152 | 0,0,152 | 475                   | 0.5850               |
| ![image](https://via.placeholder.com/15/000099/000099?) | 21845,100,153 | 0,0,153 | 478                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00009A/00009A?) | 21845,100,154 | 0,0,154 | 480                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00009B/00009B?) | 21845,100,155 | 0,0,155 | 483                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00009C/00009C?) | 21845,100,156 | 0,0,156 | 485                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00009D/00009D?) | 21845,100,157 | 0,0,157 | 488                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00009E/00009E?) | 21845,100,158 | 0,0,158 | 490                   | 0.5850               |
| ![image](https://via.placeholder.com/15/00009F/00009F?) | 21845,100,159 | 0,0,159 | 493                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000A0/0000A0?) | 21845,100,160 | 0,0,160 | 496                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000A1/0000A1?) | 21845,100,161 | 0,0,161 | 498                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000A2/0000A2?) | 21845,100,162 | 0,0,162 | 501                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000A3/0000A3?) | 21845,100,163 | 0,0,163 | 503                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000A4/0000A4?) | 21845,100,164 | 0,0,164 | 506                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000A5/0000A5?) | 21845,100,165 | 0,0,165 | 508                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000A6/0000A6?) | 21845,100,166 | 0,0,166 | 511                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000A7/0000A7?) | 21845,100,167 | 0,0,167 | 513                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000A8/0000A8?) | 21845,100,168 | 0,0,168 | 516                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000A9/0000A9?) | 21845,100,169 | 0,0,169 | 519                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000AA/0000AA?) | 21845,100,170 | 0,0,170 | 521                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000AB/0000AB?) | 21845,100,171 | 0,0,171 | 525                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000AC/0000AC?) | 21845,100,172 | 0,0,172 | 526                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000AD/0000AD?) | 21845,100,173 | 0,0,173 | 529                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000AE/0000AE?) | 21845,100,174 | 0,0,174 | 532                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000AF/0000AF?) | 21845,100,175 | 0,0,175 | 534                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000B0/0000B0?) | 21845,100,176 | 0,0,176 | 537                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000B1/0000B1?) | 21845,100,177 | 0,0,177 | 539                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000B2/0000B2?) | 21845,100,178 | 0,0,178 | 542                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000B3/0000B3?) | 21845,100,179 | 0,0,179 | 545                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000B4/0000B4?) | 21845,100,180 | 0,0,180 | 547                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000B5/0000B5?) | 21845,100,181 | 0,0,181 | 550                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000B6/0000B6?) | 21845,100,182 | 0,0,182 | 552                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000B7/0000B7?) | 21845,100,183 | 0,0,183 | 555                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000B8/0000B8?) | 21845,100,184 | 0,0,184 | 558                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000B9/0000B9?) | 21845,100,185 | 0,0,185 | 560                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000BA/0000BA?) | 21845,100,186 | 0,0,186 | 563                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000BB/0000BB?) | 21845,100,187 | 0,0,187 | 565                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000BC/0000BC?) | 21845,100,188 | 0,0,188 | 568                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000BD/0000BD?) | 21845,100,189 | 0,0,189 | 571                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000BE/0000BE?) | 21845,100,190 | 0,0,190 | 573                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000BF/0000BF?) | 21845,100,191 | 0,0,191 | 576                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000C0/0000C0?) | 21845,100,192 | 0,0,192 | 579                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000C1/0000C1?) | 21845,100,193 | 0,0,193 | 581                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000C2/0000C2?) | 21845,100,194 | 0,0,194 | 584                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000C3/0000C3?) | 21845,100,195 | 0,0,195 | 586                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000C4/0000C4?) | 21845,100,196 | 0,0,196 | 589                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000C5/0000C5?) | 21845,100,197 | 0,0,197 | 592                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000C6/0000C6?) | 21845,100,198 | 0,0,198 | 594                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000C7/0000C7?) | 21845,100,199 | 0,0,199 | 597                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000C8/0000C8?) | 21845,100,200 | 0,0,200 | 600                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000C9/0000C9?) | 21845,100,201 | 0,0,201 | 602                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000CA/0000CA?) | 21845,100,202 | 0,0,202 | 605                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000CB/0000CB?) | 21845,100,203 | 0,0,203 | 608                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000CC/0000CC?) | 21845,100,204 | 0,0,204 | 610                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000CD/0000CD?) | 21845,100,205 | 0,0,205 | 613                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000CE/0000CE?) | 21845,100,206 | 0,0,206 | 616                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000CF/0000CF?) | 21845,100,207 | 0,0,207 | 618                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000D0/0000D0?) | 21845,100,208 | 0,0,208 | 621                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000D1/0000D1?) | 21845,100,209 | 0,0,209 | 622                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000D2/0000D2?) | 21845,100,210 | 0,0,210 | 626                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000D3/0000D3?) | 21845,100,211 | 0,0,211 | 629                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000D4/0000D4?) | 21845,100,212 | 0,0,212 | 632                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000D5/0000D5?) | 21845,100,213 | 0,0,213 | 634                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000D6/0000D6?) | 21845,100,214 | 0,0,214 | 637                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000D7/0000D7?) | 21845,100,215 | 0,0,215 | 639                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000D8/0000D8?) | 21845,100,216 | 0,0,216 | 642                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000D9/0000D9?) | 21845,100,217 | 0,0,217 | 645                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000DA/0000DA?) | 21845,100,218 | 0,0,218 | 647                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000DB/0000DB?) | 21845,100,219 | 0,0,219 | 650                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000DC/0000DC?) | 21845,100,220 | 0,0,220 | 653                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000DD/0000DD?) | 21845,100,221 | 0,0,221 | 655                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000DE/0000DE?) | 21845,100,222 | 0,0,222 | 658                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000DF/0000DF?) | 21845,100,223 | 0,0,223 | 661                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000E0/0000E0?) | 21845,100,224 | 0,0,224 | 663                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000E1/0000E1?) | 21845,100,225 | 0,0,225 | 666                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000E2/0000E2?) | 21845,100,226 | 0,0,226 | 669                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000E3/0000E3?) | 21845,100,227 | 0,0,227 | 671                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000E4/0000E4?) | 21845,100,228 | 0,0,228 | 674                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000E5/0000E5?) | 21845,100,229 | 0,0,229 | 676                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000E6/0000E6?) | 21845,100,230 | 0,0,230 | 679                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000E7/0000E7?) | 21845,100,231 | 0,0,231 | 682                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000E8/0000E8?) | 21845,100,232 | 0,0,232 | 684                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000E9/0000E9?) | 21845,100,233 | 0,0,233 | 687                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000EA/0000EA?) | 21845,100,234 | 0,0,234 | 690                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000EB/0000EB?) | 21845,100,235 | 0,0,235 | 692                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000EC/0000EC?) | 21845,100,236 | 0,0,236 | 695                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000ED/0000ED?) | 21845,100,237 | 0,0,237 | 698                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000EE/0000EE?) | 21845,100,238 | 0,0,238 | 700                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000EF/0000EF?) | 21845,100,239 | 0,0,239 | 703                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000F0/0000F0?) | 21845,100,240 | 0,0,240 | 705                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000F1/0000F1?) | 21845,100,241 | 0,0,241 | 708                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000F2/0000F2?) | 21845,100,242 | 0,0,242 | 711                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000F3/0000F3?) | 21845,100,243 | 0,0,243 | 713                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000F4/0000F4?) | 21845,100,244 | 0,0,244 | 716                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000F5/0000F5?) | 21845,100,245 | 0,0,245 | 719                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000F6/0000F6?) | 21845,100,246 | 0,0,246 | 721                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000F7/0000F7?) | 21845,100,247 | 0,0,247 | 724                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000F8/0000F8?) | 21845,100,248 | 0,0,248 | 727                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000F9/0000F9?) | 21845,100,249 | 0,0,249 | 729                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000FA/0000FA?) | 21845,100,250 | 0,0,250 | 732                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000FB/0000FB?) | 21845,100,251 | 0,0,251 | 734                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000FC/0000FC?) | 21845,100,252 | 0,0,252 | 737                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000FD/0000FD?) | 21845,100,253 | 0,0,253 | 740                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000FE/0000FE?) | 21845,100,254 | 0,0,254 | 742                   | 0.5850               |
| ![image](https://via.placeholder.com/15/0000FF/0000FF?) | 21845,100,255 | 0,0,255 | 748                   | 0.5850               |

### White LED Testing (cut short due to obvious current limitations)
| Swatch                                                  | HSV           | RGB         | Measured Current (mA) | Current Per LED (mA) |
|---------------------------------------------------------|---------------|-------------|-----------------------|----------------------|
| ![image](https://via.placeholder.com/15/000000/000000?) | 21845,100,0   | 0,0,0       | 86                    | 0.5850               |
| ![image](https://via.placeholder.com/15/010101/010101?) | 21845,100,1   | 1,1,1       | 93                    | 0.6028               |
| ![image](https://via.placeholder.com/15/020202/020202?) | 21845,100,2   | 2,2,2       | 101                   | 0.6207               |
| ![image](https://via.placeholder.com/15/030303/030303?) | 21845,100,3   | 3,3,3       | 109                   | 0.6385               |
| ![image](https://via.placeholder.com/15/040404/040404?) | 21845,100,4   | 4,4,4       | 116                   | 0.6563               |
| ![image](https://via.placeholder.com/15/050505/050505?) | 21845,100,5   | 5,5,5       | 124                   | 0.6741               |
| ![image](https://via.placeholder.com/15/060606/060606?) | 21845,100,6   | 6,6,6       | 132                   | 0.6919               |
| ![image](https://via.placeholder.com/15/070707/070707?) | 21845,100,7   | 7,7,7       | 139                   | 0.7097               |
| ![image](https://via.placeholder.com/15/080808/080808?) | 21845,100,8   | 8,8,8       | 147                   | 0.7275               |
| ![image](https://via.placeholder.com/15/090909/090909?) | 21845,100,9   | 9,9,9       | 155                   | 0.7454               |
| ![image](https://via.placeholder.com/15/0A0A0A/0A0A0A?) | 21845,100,10  | 10,10,10    | 162                   | 0.7632               |
| ![image](https://via.placeholder.com/15/0B0B0B/0B0B0B?) | 21845,100,11  | 11,11,11    | 170                   | 0.7810               |
| ![image](https://via.placeholder.com/15/0C0C0C/0C0C0C?) | 21845,100,12  | 12,12,12    | 177                   | 0.7988               |
| ![image](https://via.placeholder.com/15/0D0D0D/0D0D0D?) | 21845,100,13  | 13,13,13    | 186                   | 0.8166               |
| ![image](https://via.placeholder.com/15/0E0E0E/0E0E0E?) | 21845,100,14  | 14,14,14    | 193                   | 0.8344               |
| ![image](https://via.placeholder.com/15/0F0F0F/0F0F0F?) | 21845,100,15  | 15,15,15    | 201                   | 0.8522               |
| ![image](https://via.placeholder.com/15/101010/101010?) | 21845,100,16  | 16,16,16    | 209                   | 0.8701               |
| ![image](https://via.placeholder.com/15/111111/111111?) | 21845,100,17  | 17,17,17    | 216                   | 0.8879               |
| ![image](https://via.placeholder.com/15/121212/121212?) | 21845,100,18  | 18,18,18    | 224                   | 0.9057               |
| ![image](https://via.placeholder.com/15/131313/131313?) | 21845,100,19  | 19,19,19    | 232                   | 0.9235               |
| ![image](https://via.placeholder.com/15/141414/141414?) | 21845,100,20  | 20,20,20    | 240                   | 0.9413               |
| ![image](https://via.placeholder.com/15/151515/151515?) | 21845,100,21  | 21,21,21    | 247                   | 0.9591               |
| ![image](https://via.placeholder.com/15/161616/161616?) | 21845,100,22  | 22,22,22    | 255                   | 0.9769               |
| ![image](https://via.placeholder.com/15/171717/171717?) | 21845,100,23  | 23,23,23    | 263                   | 0.9948               |
| ![image](https://via.placeholder.com/15/181818/181818?) | 21845,100,24  | 24,24,24    | 270                   | 1.0126               |
| ![image](https://via.placeholder.com/15/191919/191919?) | 21845,100,25  | 25,25,25    | 278                   | 1.0304               |
| ![image](https://via.placeholder.com/15/1A1A1A/1A1A1A?) | 21845,100,26  | 26,26,26    | 286                   | 1.0482               |
| ![image](https://via.placeholder.com/15/1B1B1B/1B1B1B?) | 21845,100,27  | 27,27,27    | 293                   | 1.0660               |
| ![image](https://via.placeholder.com/15/1C1C1C/1C1C1C?) | 21845,100,28  | 28,28,28    | 300                   | 1.0838               |
| ![image](https://via.placeholder.com/15/1D1D1D/1D1D1D?) | 21845,100,29  | 29,29,29    | 309                   | 1.1016               |
| ![image](https://via.placeholder.com/15/1E1E1E/1E1E1E?) | 21845,100,30  | 30,30,30    | 316                   | 1.1194               |
| ![image](https://via.placeholder.com/15/1F1F1F/1F1F1F?) | 21845,100,31  | 31,31,31    | 324                   | 1.1373               |
| ![image](https://via.placeholder.com/15/202020/202020?) | 21845,100,32  | 32,32,32    | 332                   | 1.1551               |
| ![image](https://via.placeholder.com/15/212121/212121?) | 21845,100,33  | 33,33,33    | 340                   | 1.1729               |
| ![image](https://via.placeholder.com/15/222222/222222?) | 21845,100,34  | 34,34,34    | 347                   | 1.1907               |
| ![image](https://via.placeholder.com/15/232323/232323?) | 21845,100,35  | 35,35,35    | 355                   | 1.2085               |
| ![image](https://via.placeholder.com/15/242424/242424?) | 21845,100,36  | 36,36,36    | 363                   | 1.2263               |
| ![image](https://via.placeholder.com/15/252525/252525?) | 21845,100,37  | 37,37,37    | 370                   | 1.2441               |
| ![image](https://via.placeholder.com/15/262626/262626?) | 21845,100,38  | 38,38,38    | 378                   | 1.2620               |
| ![image](https://via.placeholder.com/15/272727/272727?) | 21845,100,39  | 39,39,39    | 386                   | 1.2798               |
| ![image](https://via.placeholder.com/15/282828/282828?) | 21845,100,40  | 40,40,40    | 393                   | 1.2976               |
| ![image](https://via.placeholder.com/15/292929/292929?) | 21845,100,41  | 41,41,41    | 401                   | 1.3154               |
| ![image](https://via.placeholder.com/15/2A2A2A/2A2A2A?) | 21845,100,42  | 42,42,42    | 409                   | 1.3332               |
| ![image](https://via.placeholder.com/15/2B2B2B/2B2B2B?) | 21845,100,43  | 43,43,43    | 417                   | 1.3510               |
| ![image](https://via.placeholder.com/15/2C2C2C/2C2C2C?) | 21845,100,44  | 44,44,44    | 424                   | 1.3688               |
| ![image](https://via.placeholder.com/15/2D2D2D/2D2D2D?) | 21845,100,45  | 45,45,45    | 432                   | 1.3867               |
| ![image](https://via.placeholder.com/15/2E2E2E/2E2E2E?) | 21845,100,46  | 46,46,46    | 440                   | 1.4045               |
| ![image](https://via.placeholder.com/15/2F2F2F/2F2F2F?) | 21845,100,47  | 47,47,47    | 447                   | 1.4223               |
| ![image](https://via.placeholder.com/15/303030/303030?) | 21845,100,48  | 48,48,48    | 455                   | 1.4401               |
| ![image](https://via.placeholder.com/15/313131/313131?) | 21845,100,49  | 49,49,49    | 463                   | 1.4579               |
| ![image](https://via.placeholder.com/15/323232/323232?) | 21845,100,50  | 50,50,50    | 470                   | 1.4757               |
| ![image](https://via.placeholder.com/15/333333/333333?) | 21845,100,51  | 51,51,51    | 478                   | 1.4935               |
| ![image](https://via.placeholder.com/15/343434/343434?) | 21845,100,52  | 52,52,52    | 486                   | 1.5114               |
| ![image](https://via.placeholder.com/15/353535/353535?) | 21845,100,53  | 53,53,53    | 494                   | 1.5292               |
| ![image](https://via.placeholder.com/15/363636/363636?) | 21845,100,54  | 54,54,54    | 502                   | 1.5470               |
| ![image](https://via.placeholder.com/15/373737/373737?) | 21845,100,55  | 55,55,55    | 509                   | 1.5648               |
| ![image](https://via.placeholder.com/15/383838/383838?) | 21845,100,56  | 56,56,56    | 517                   | 1.5826               |
| ![image](https://via.placeholder.com/15/393939/393939?) | 21845,100,57  | 57,57,57    | 525                   | 1.6004               |
| ![image](https://via.placeholder.com/15/3A3A3A/3A3A3A?) | 21845,100,58  | 58,58,58    | 533                   | 1.6182               |
| ![image](https://via.placeholder.com/15/3B3B3B/3B3B3B?) | 21845,100,59  | 59,59,59    | 540                   | 1.6360               |
| ![image](https://via.placeholder.com/15/3C3C3C/3C3C3C?) | 21845,100,60  | 60,60,60    | 548                   | 1.6539               |
| ![image](https://via.placeholder.com/15/3D3D3D/3D3D3D?) | 21845,100,61  | 61,61,61    | 556                   | 1.6717               |
| ![image](https://via.placeholder.com/15/3E3E3E/3E3E3E?) | 21845,100,62  | 62,62,62    | 564                   | 1.6895               |
| ![image](https://via.placeholder.com/15/3F3F3F/3F3F3F?) | 21845,100,63  | 63,63,63    | 572                   | 1.7073               |
| ![image](https://via.placeholder.com/15/404040/404040?) | 21845,100,64  | 64,64,64    | 580                   | 1.7251               |
| ![image](https://via.placeholder.com/15/414141/414141?) | 21845,100,65  | 65,65,65    | 588                   | 1.7429               |
| ![image](https://via.placeholder.com/15/424242/424242?) | 21845,100,66  | 66,66,66    | 595                   | 1.7607               |
| ![image](https://via.placeholder.com/15/434343/434343?) | 21845,100,67  | 67,67,67    | 603                   | 1.7786               |
| ![image](https://via.placeholder.com/15/444444/444444?) | 21845,100,68  | 68,68,68    | 611                   | 1.7964               |
| ![image](https://via.placeholder.com/15/454545/454545?) | 21845,100,69  | 69,69,69    | 619                   | 1.8142               |
| ![image](https://via.placeholder.com/15/464646/464646?) | 21845,100,70  | 70,70,70    | 615                   | 1.8320               |
| ![image](https://via.placeholder.com/15/474747/474747?) | 21845,100,71  | 71,71,71    | 635                   | 1.8498               |
| ![image](https://via.placeholder.com/15/484848/484848?) | 21845,100,72  | 72,72,72    | 643                   | 1.8676               |
| ![image](https://via.placeholder.com/15/494949/494949?) | 21845,100,73  | 73,73,73    | 650                   | 1.8854               |
| ![image](https://via.placeholder.com/15/4A4A4A/4A4A4A?) | 21845,100,74  | 74,74,74    | 658                   | 1.9033               |
| ![image](https://via.placeholder.com/15/4B4B4B/4B4B4B?) | 21845,100,75  | 75,75,75    | 666                   | 1.9211               |
| ![image](https://via.placeholder.com/15/4C4C4C/4C4C4C?) | 21845,100,76  | 76,76,76    | 674                   | 1.9389               |
| ![image](https://via.placeholder.com/15/4D4D4D/4D4D4D?) | 21845,100,77  | 77,77,77    | 682                   | 1.9567               |
| ![image](https://via.placeholder.com/15/4E4E4E/4E4E4E?) | 21845,100,78  | 78,78,78    | 690                   | 1.9745               |
| ![image](https://via.placeholder.com/15/4F4F4F/4F4F4F?) | 21845,100,79  | 79,79,79    | 698                   | 1.9923               |
| ![image](https://via.placeholder.com/15/505050/505050?) | 21845,100,80  | 80,80,80    | 700                   | 2.0101               |
| ![image](https://via.placeholder.com/15/515151/515151?) | 21845,100,81  | 81,81,81    | 706                   | 2.0280               |
| ![image](https://via.placeholder.com/15/525252/525252?) | 21845,100,82  | 82,82,82    | 713                   | 2.0458               |
| ![image](https://via.placeholder.com/15/535353/535353?) | 21845,100,83  | 83,83,83    | 722                   | 2.0636               |
| ![image](https://via.placeholder.com/15/545454/545454?) | 21845,100,84  | 84,84,84    | 729                   | 2.0814               |
| ![image](https://via.placeholder.com/15/555555/555555?) | 21845,100,85  | 85,85,85    | 737                   | 2.0992               |
| ![image](https://via.placeholder.com/15/565656/565656?) | 21845,100,86  | 86,86,86    | 745                   | 2.1170               |
| ![image](https://via.placeholder.com/15/575757/575757?) | 21845,100,87  | 87,87,87    | 753                   | 2.1348               |
| ![image](https://via.placeholder.com/15/585858/585858?) | 21845,100,88  | 88,88,88    | 760                   | 2.1526               |
| ![image](https://via.placeholder.com/15/595959/595959?) | 21845,100,89  | 89,89,89    | 768                   | 2.1705               |
| ![image](https://via.placeholder.com/15/5A5A5A/5A5A5A?) | 21845,100,90  | 90,90,90    | 776                   | 2.1883               |
| ![image](https://via.placeholder.com/15/5B5B5B/5B5B5B?) | 21845,100,91  | 91,91,91    | 784                   | 2.2061               |
| ![image](https://via.placeholder.com/15/5C5C5C/5C5C5C?) | 21845,100,92  | 92,92,92    | 791                   | 2.2239               |
| ![image](https://via.placeholder.com/15/5D5D5D/5D5D5D?) | 21845,100,93  | 93,93,93    | 799                   | 2.2417               |
| ![image](https://via.placeholder.com/15/5E5E5E/5E5E5E?) | 21845,100,94  | 94,94,94    | 800                   | 2.2595               |
| ![image](https://via.placeholder.com/15/5F5F5F/5F5F5F?) | 21845,100,95  | 95,95,95    | 806                   | 2.2773               |
| ![image](https://via.placeholder.com/15/606060/606060?) | 21845,100,96  | 96,96,96    | 811                   | 2.2952               |
| ![image](https://via.placeholder.com/15/616161/616161?) | 21845,100,97  | 97,97,97    | 821                   | 2.3130               |
| ![image](https://via.placeholder.com/15/626262/626262?) | 21845,100,98  | 98,98,98    | 829                   | 2.3308               |
| ![image](https://via.placeholder.com/15/636363/636363?) | 21845,100,99  | 99,99,99    | 832                   | 2.3486               |
| ![image](https://via.placeholder.com/15/646464/646464?) | 21845,100,100 | 100,100,100 | 841                   | 2.3664               |
| ![image](https://via.placeholder.com/15/656565/656565?) | 21845,100,101 | 101,101,101 | 849                   | 2.3842               |
| ![image](https://via.placeholder.com/15/666666/666666?) | 21845,100,102 | 102,102,102 | 855                   | 2.4020               |
| ![image](https://via.placeholder.com/15/676767/676767?) | 21845,100,103 | 103,103,103 | 860                   | 2.4199               |
| ![image](https://via.placeholder.com/15/686868/686868?) | 21845,100,104 | 104,104,104 | 864                   | 2.4377               |
| ![image](https://via.placeholder.com/15/696969/696969?) | 21845,100,105 | 105,105,105 | 874                   | 2.4555               |
| ![image](https://via.placeholder.com/15/6A6A6A/6A6A6A?) | 21845,100,106 | 106,106,106 | 877                   | 2.4733               |
| ![image](https://via.placeholder.com/15/6B6B6B/6B6B6B?) | 21845,100,107 | 107,107,107 | 884                   | 2.4911               |
| ![image](https://via.placeholder.com/15/6C6C6C/6C6C6C?) | 21845,100,108 | 108,108,108 |                       | 2.5089               |
| ![image](https://via.placeholder.com/15/6D6D6D/6D6D6D?) | 21845,100,109 | 109,109,109 |                       | 2.5267               |
| ![image](https://via.placeholder.com/15/6E6E6E/6E6E6E?) | 21845,100,110 | 110,110,110 | 905                   | 2.5446               |
| ![image](https://via.placeholder.com/15/6F6F6F/6F6F6F?) | 21845,100,111 | 111,111,111 |                       | 2.5624               |
| ![image](https://via.placeholder.com/15/707070/707070?) | 21845,100,112 | 112,112,112 |                       | 2.5802               |
| ![image](https://via.placeholder.com/15/717171/717171?) | 21845,100,113 | 113,113,113 |                       | 2.5980               |
| ![image](https://via.placeholder.com/15/727272/727272?) | 21845,100,114 | 114,114,114 |                       | 2.6158               |
| ![image](https://via.placeholder.com/15/737373/737373?) | 21845,100,115 | 115,115,115 | 927                   | 2.6336               |
| ![image](https://via.placeholder.com/15/747474/747474?) | 21845,100,116 | 116,116,116 |                       | 2.6514               |
| ![image](https://via.placeholder.com/15/757575/757575?) | 21845,100,117 | 117,117,117 |                       | 2.6692               |
| ![image](https://via.placeholder.com/15/767676/767676?) | 21845,100,118 | 118,118,118 |                       | 2.6871               |
| ![image](https://via.placeholder.com/15/777777/777777?) | 21845,100,119 | 119,119,119 |                       | 2.7049               |
| ![image](https://via.placeholder.com/15/787878/787878?) | 21845,100,120 | 120,120,120 | 940                   | 2.7227               |
| ![image](https://via.placeholder.com/15/797979/797979?) | 21845,100,121 | 121,121,121 |                       | 2.7405               |
| ![image](https://via.placeholder.com/15/7A7A7A/7A7A7A?) | 21845,100,122 | 122,122,122 |                       | 2.7583               |
| ![image](https://via.placeholder.com/15/7B7B7B/7B7B7B?) | 21845,100,123 | 123,123,123 |                       | 2.7761               |
| ![image](https://via.placeholder.com/15/7C7C7C/7C7C7C?) | 21845,100,124 | 124,124,124 |                       | 2.7939               |
| ![image](https://via.placeholder.com/15/7D7D7D/7D7D7D?) | 21845,100,125 | 125,125,125 | 952                   | 2.8118               |
| ![image](https://via.placeholder.com/15/7E7E7E/7E7E7E?) | 21845,100,126 | 126,126,126 |                       | 2.8296               |
| ![image](https://via.placeholder.com/15/7F7F7F/7F7F7F?) | 21845,100,127 | 127,127,127 |                       | 2.8474               |
| ![image](https://via.placeholder.com/15/808080/808080?) | 21845,100,128 | 128,128,128 |                       | 2.8652               |
| ![image](https://via.placeholder.com/15/818181/818181?) | 21845,100,129 | 129,129,129 |                       | 2.8830               |
| ![image](https://via.placeholder.com/15/828282/828282?) | 21845,100,130 | 130,130,130 | 961                   | 2.9008               |
| ![image](https://via.placeholder.com/15/838383/838383?) | 21845,100,131 | 131,131,131 |                       | 2.9186               |
| ![image](https://via.placeholder.com/15/848484/848484?) | 21845,100,132 | 132,132,132 |                       | 2.9365               |
| ![image](https://via.placeholder.com/15/858585/858585?) | 21845,100,133 | 133,133,133 |                       | 2.9543               |
| ![image](https://via.placeholder.com/15/868686/868686?) | 21845,100,134 | 134,134,134 |                       | 2.9721               |
| ![image](https://via.placeholder.com/15/878787/878787?) | 21845,100,135 | 135,135,135 | 970                   | 2.9899               |
| ![image](https://via.placeholder.com/15/888888/888888?) | 21845,100,136 | 136,136,136 |                       | 3.0077               |
| ![image](https://via.placeholder.com/15/898989/898989?) | 21845,100,137 | 137,137,137 |                       | 3.0255               |
| ![image](https://via.placeholder.com/15/8A8A8A/8A8A8A?) | 21845,100,138 | 138,138,138 |                       | 3.0433               |
| ![image](https://via.placeholder.com/15/8B8B8B/8B8B8B?) | 21845,100,139 | 139,139,139 |                       | 3.0612               |
| ![image](https://via.placeholder.com/15/8C8C8C/8C8C8C?) | 21845,100,140 | 140,140,140 | 975                   | 3.0790               |
| ![image](https://via.placeholder.com/15/8D8D8D/8D8D8D?) | 21845,100,141 | 141,141,141 |                       | 3.0968               |
| ![image](https://via.placeholder.com/15/8E8E8E/8E8E8E?) | 21845,100,142 | 142,142,142 |                       | 3.1146               |
| ![image](https://via.placeholder.com/15/8F8F8F/8F8F8F?) | 21845,100,143 | 143,143,143 |                       | 3.1324               |
| ![image](https://via.placeholder.com/15/909090/909090?) | 21845,100,144 | 144,144,144 |                       | 3.1502               |
| ![image](https://via.placeholder.com/15/919191/919191?) | 21845,100,145 | 145,145,145 | 982                   | 3.1680               |
| ![image](https://via.placeholder.com/15/929292/929292?) | 21845,100,146 | 146,146,146 |                       | 3.1858               |
| ![image](https://via.placeholder.com/15/939393/939393?) | 21845,100,147 | 147,147,147 |                       | 3.2037               |
| ![image](https://via.placeholder.com/15/949494/949494?) | 21845,100,148 | 148,148,148 |                       | 3.2215               |
| ![image](https://via.placeholder.com/15/959595/959595?) | 21845,100,149 | 149,149,149 |                       | 3.2393               |
| ![image](https://via.placeholder.com/15/969696/969696?) | 21845,100,150 | 150,150,150 | 988                   | 3.2571               |
| ![image](https://via.placeholder.com/15/979797/979797?) | 21845,100,151 | 151,151,151 |                       | 3.2749               |
| ![image](https://via.placeholder.com/15/989898/989898?) | 21845,100,152 | 152,152,152 |                       | 3.2927               |
| ![image](https://via.placeholder.com/15/999999/999999?) | 21845,100,153 | 153,153,153 |                       | 3.3105               |
| ![image](https://via.placeholder.com/15/9A9A9A/9A9A9A?) | 21845,100,154 | 154,154,154 |                       | 3.3284               |
| ![image](https://via.placeholder.com/15/9B9B9B/9B9B9B?) | 21845,100,155 | 155,155,155 | 993                   | 3.3462               |
| ![image](https://via.placeholder.com/15/9C9C9C/9C9C9C?) | 21845,100,156 | 156,156,156 |                       | 3.3640               |
| ![image](https://via.placeholder.com/15/9D9D9D/9D9D9D?) | 21845,100,157 | 157,157,157 |                       | 3.3818               |
| ![image](https://via.placeholder.com/15/9E9E9E/9E9E9E?) | 21845,100,158 | 158,158,158 |                       | 3.3996               |
| ![image](https://via.placeholder.com/15/9F9F9F/9F9F9F?) | 21845,100,159 | 159,159,159 |                       | 3.4174               |
| ![image](https://via.placeholder.com/15/A0A0A0/A0A0A0?) | 21845,100,160 | 160,160,160 | 998                   | 3.4352               |
| ![image](https://via.placeholder.com/15/A1A1A1/A1A1A1?) | 21845,100,161 | 161,161,161 |                       | 3.4531               |
| ![image](https://via.placeholder.com/15/A2A2A2/A2A2A2?) | 21845,100,162 | 162,162,162 |                       | 3.4709               |
| ![image](https://via.placeholder.com/15/A3A3A3/A3A3A3?) | 21845,100,163 | 163,163,163 |                       | 3.4887               |
| ![image](https://via.placeholder.com/15/A4A4A4/A4A4A4?) | 21845,100,164 | 164,164,164 |                       | 3.5065               |
| ![image](https://via.placeholder.com/15/A5A5A5/A5A5A5?) | 21845,100,165 | 165,165,165 | 1003                  | 3.5243               |
| ![image](https://via.placeholder.com/15/A6A6A6/A6A6A6?) | 21845,100,166 | 166,166,166 |                       | 3.5421               |
| ![image](https://via.placeholder.com/15/A7A7A7/A7A7A7?) | 21845,100,167 | 167,167,167 |                       | 3.5599               |
| ![image](https://via.placeholder.com/15/A8A8A8/A8A8A8?) | 21845,100,168 | 168,168,168 |                       | 3.5778               |
| ![image](https://via.placeholder.com/15/A9A9A9/A9A9A9?) | 21845,100,169 | 169,169,169 |                       | 3.5956               |
| ![image](https://via.placeholder.com/15/AAAAAA/AAAAAA?) | 21845,100,170 | 170,170,170 | 1008                  | 3.6134               |
| ![image](https://via.placeholder.com/15/ABABAB/ABABAB?) | 21845,100,171 | 171,171,171 |                       | 3.6312               |
| ![image](https://via.placeholder.com/15/ACACAC/ACACAC?) | 21845,100,172 | 172,172,172 |                       | 3.6490               |
| ![image](https://via.placeholder.com/15/ADADAD/ADADAD?) | 21845,100,173 | 173,173,173 |                       | 3.6668               |
| ![image](https://via.placeholder.com/15/AEAEAE/AEAEAE?) | 21845,100,174 | 174,174,174 |                       | 3.6846               |
| ![image](https://via.placeholder.com/15/AFAFAF/AFAFAF?) | 21845,100,175 | 175,175,175 | 1012                  | 3.7024               |
| ![image](https://via.placeholder.com/15/B0B0B0/B0B0B0?) | 21845,100,176 | 176,176,176 |                       | 3.7203               |
| ![image](https://via.placeholder.com/15/B1B1B1/B1B1B1?) | 21845,100,177 | 177,177,177 |                       | 3.7381               |
| ![image](https://via.placeholder.com/15/B2B2B2/B2B2B2?) | 21845,100,178 | 178,178,178 |                       | 3.7559               |
| ![image](https://via.placeholder.com/15/B3B3B3/B3B3B3?) | 21845,100,179 | 179,179,179 |                       | 3.7737               |
| ![image](https://via.placeholder.com/15/B4B4B4/B4B4B4?) | 21845,100,180 | 180,180,180 | 1016                  | 3.7915               |
| ![image](https://via.placeholder.com/15/B5B5B5/B5B5B5?) | 21845,100,181 | 181,181,181 |                       | 3.8093               |
| ![image](https://via.placeholder.com/15/B6B6B6/B6B6B6?) | 21845,100,182 | 182,182,182 |                       | 3.8271               |
| ![image](https://via.placeholder.com/15/B7B7B7/B7B7B7?) | 21845,100,183 | 183,183,183 |                       | 3.8450               |
| ![image](https://via.placeholder.com/15/B8B8B8/B8B8B8?) | 21845,100,184 | 184,184,184 |                       | 3.8628               |
| ![image](https://via.placeholder.com/15/B9B9B9/B9B9B9?) | 21845,100,185 | 185,185,185 | 1020                  | 3.8806               |
| ![image](https://via.placeholder.com/15/BABABA/BABABA?) | 21845,100,186 | 186,186,186 |                       | 3.8984               |
| ![image](https://via.placeholder.com/15/BBBBBB/BBBBBB?) | 21845,100,187 | 187,187,187 |                       | 3.9162               |
| ![image](https://via.placeholder.com/15/BCBCBC/BCBCBC?) | 21845,100,188 | 188,188,188 |                       | 3.9340               |
| ![image](https://via.placeholder.com/15/BDBDBD/BDBDBD?) | 21845,100,189 | 189,189,189 |                       | 3.9518               |
| ![image](https://via.placeholder.com/15/BEBEBE/BEBEBE?) | 21845,100,190 | 190,190,190 | 1023                  | 3.9697               |
| ![image](https://via.placeholder.com/15/BFBFBF/BFBFBF?) | 21845,100,191 | 191,191,191 |                       | 3.9875               |
| ![image](https://via.placeholder.com/15/C0C0C0/C0C0C0?) | 21845,100,192 | 192,192,192 |                       | 4.0053               |
| ![image](https://via.placeholder.com/15/C1C1C1/C1C1C1?) | 21845,100,193 | 193,193,193 |                       | 4.0231               |
| ![image](https://via.placeholder.com/15/C2C2C2/C2C2C2?) | 21845,100,194 | 194,194,194 |                       | 4.0409               |
| ![image](https://via.placeholder.com/15/C3C3C3/C3C3C3?) | 21845,100,195 | 195,195,195 | 1027                  | 4.0587               |
| ![image](https://via.placeholder.com/15/C4C4C4/C4C4C4?) | 21845,100,196 | 196,196,196 |                       | 4.0765               |
| ![image](https://via.placeholder.com/15/C5C5C5/C5C5C5?) | 21845,100,197 | 197,197,197 |                       | 4.0944               |
| ![image](https://via.placeholder.com/15/C6C6C6/C6C6C6?) | 21845,100,198 | 198,198,198 |                       | 4.1122               |
| ![image](https://via.placeholder.com/15/C7C7C7/C7C7C7?) | 21845,100,199 | 199,199,199 |                       | 4.1300               |
| ![image](https://via.placeholder.com/15/C8C8C8/C8C8C8?) | 21845,100,200 | 200,200,200 | 1031                  | 4.1478               |
| ![image](https://via.placeholder.com/15/C9C9C9/C9C9C9?) | 21845,100,201 | 201,201,201 |                       | 4.1656               |
| ![image](https://via.placeholder.com/15/CACACA/CACACA?) | 21845,100,202 | 202,202,202 |                       | 4.1834               |
| ![image](https://via.placeholder.com/15/CBCBCB/CBCBCB?) | 21845,100,203 | 203,203,203 |                       | 4.2012               |
| ![image](https://via.placeholder.com/15/CCCCCC/CCCCCC?) | 21845,100,204 | 204,204,204 |                       | 4.2190               |
| ![image](https://via.placeholder.com/15/CDCDCD/CDCDCD?) | 21845,100,205 | 205,205,205 | 1034                  | 4.2369               |
| ![image](https://via.placeholder.com/15/CECECE/CECECE?) | 21845,100,206 | 206,206,206 |                       | 4.2547               |
| ![image](https://via.placeholder.com/15/CFCFCF/CFCFCF?) | 21845,100,207 | 207,207,207 |                       | 4.2725               |
| ![image](https://via.placeholder.com/15/D0D0D0/D0D0D0?) | 21845,100,208 | 208,208,208 |                       | 4.2903               |
| ![image](https://via.placeholder.com/15/D1D1D1/D1D1D1?) | 21845,100,209 | 209,209,209 |                       | 4.3081               |
| ![image](https://via.placeholder.com/15/D2D2D2/D2D2D2?) | 21845,100,210 | 210,210,210 | 1037                  | 4.3259               |
| ![image](https://via.placeholder.com/15/D3D3D3/D3D3D3?) | 21845,100,211 | 211,211,211 |                       | 4.3437               |
| ![image](https://via.placeholder.com/15/D4D4D4/D4D4D4?) | 21845,100,212 | 212,212,212 |                       | 4.3616               |
| ![image](https://via.placeholder.com/15/D5D5D5/D5D5D5?) | 21845,100,213 | 213,213,213 |                       | 4.3794               |
| ![image](https://via.placeholder.com/15/D6D6D6/D6D6D6?) | 21845,100,214 | 214,214,214 |                       | 4.3972               |
| ![image](https://via.placeholder.com/15/D7D7D7/D7D7D7?) | 21845,100,215 | 215,215,215 | 1041                  | 4.4150               |
| ![image](https://via.placeholder.com/15/D8D8D8/D8D8D8?) | 21845,100,216 | 216,216,216 |                       | 4.4328               |
| ![image](https://via.placeholder.com/15/D9D9D9/D9D9D9?) | 21845,100,217 | 217,217,217 |                       | 4.4506               |
| ![image](https://via.placeholder.com/15/DADADA/DADADA?) | 21845,100,218 | 218,218,218 |                       | 4.4684               |
| ![image](https://via.placeholder.com/15/DBDBDB/DBDBDB?) | 21845,100,219 | 219,219,219 |                       | 4.4863               |
| ![image](https://via.placeholder.com/15/DCDCDC/DCDCDC?) | 21845,100,220 | 220,220,220 | 1044                  | 4.5041               |
| ![image](https://via.placeholder.com/15/DDDDDD/DDDDDD?) | 21845,100,221 | 221,221,221 |                       | 4.5219               |
| ![image](https://via.placeholder.com/15/DEDEDE/DEDEDE?) | 21845,100,222 | 222,222,222 |                       | 4.5397               |
| ![image](https://via.placeholder.com/15/DFDFDF/DFDFDF?) | 21845,100,223 | 223,223,223 |                       | 4.5575               |
| ![image](https://via.placeholder.com/15/E0E0E0/E0E0E0?) | 21845,100,224 | 224,224,224 |                       | 4.5753               |
| ![image](https://via.placeholder.com/15/E1E1E1/E1E1E1?) | 21845,100,225 | 225,225,225 | 1047                  | 4.5931               |
| ![image](https://via.placeholder.com/15/E2E2E2/E2E2E2?) | 21845,100,226 | 226,226,226 |                       | 4.6110               |
| ![image](https://via.placeholder.com/15/E3E3E3/E3E3E3?) | 21845,100,227 | 227,227,227 |                       | 4.6288               |
| ![image](https://via.placeholder.com/15/E4E4E4/E4E4E4?) | 21845,100,228 | 228,228,228 |                       | 4.6466               |
| ![image](https://via.placeholder.com/15/E5E5E5/E5E5E5?) | 21845,100,229 | 229,229,229 |                       | 4.6644               |
| ![image](https://via.placeholder.com/15/E6E6E6/E6E6E6?) | 21845,100,230 | 230,230,230 | 1050                  | 4.6822               |
| ![image](https://via.placeholder.com/15/E7E7E7/E7E7E7?) | 21845,100,231 | 231,231,231 |                       | 4.7000               |
| ![image](https://via.placeholder.com/15/E8E8E8/E8E8E8?) | 21845,100,232 | 232,232,232 |                       | 4.7178               |
| ![image](https://via.placeholder.com/15/E9E9E9/E9E9E9?) | 21845,100,233 | 233,233,233 |                       | 4.7357               |
| ![image](https://via.placeholder.com/15/EAEAEA/EAEAEA?) | 21845,100,234 | 234,234,234 |                       | 4.7535               |
| ![image](https://via.placeholder.com/15/EBEBEB/EBEBEB?) | 21845,100,235 | 235,235,235 | 1053                  | 4.7713               |
| ![image](https://via.placeholder.com/15/ECECEC/ECECEC?) | 21845,100,236 | 236,236,236 |                       | 4.7891               |
| ![image](https://via.placeholder.com/15/EDEDED/EDEDED?) | 21845,100,237 | 237,237,237 |                       | 4.8069               |
| ![image](https://via.placeholder.com/15/EEEEEE/EEEEEE?) | 21845,100,238 | 238,238,238 |                       | 4.8247               |
| ![image](https://via.placeholder.com/15/EFEFEF/EFEFEF?) | 21845,100,239 | 239,239,239 |                       | 4.8425               |
| ![image](https://via.placeholder.com/15/F0F0F0/F0F0F0?) | 21845,100,240 | 240,240,240 | 1056                  | 4.8603               |
| ![image](https://via.placeholder.com/15/F1F1F1/F1F1F1?) | 21845,100,241 | 241,241,241 |                       | 4.8782               |
| ![image](https://via.placeholder.com/15/F2F2F2/F2F2F2?) | 21845,100,242 | 242,242,242 |                       | 4.8960               |
| ![image](https://via.placeholder.com/15/F3F3F3/F3F3F3?) | 21845,100,243 | 243,243,243 |                       | 4.9138               |
| ![image](https://via.placeholder.com/15/F4F4F4/F4F4F4?) | 21845,100,244 | 244,244,244 |                       | 4.9316               |
| ![image](https://via.placeholder.com/15/F5F5F5/F5F5F5?) | 21845,100,245 | 245,245,245 | 1058                  | 4.9494               |
| ![image](https://via.placeholder.com/15/F6F6F6/F6F6F6?) | 21845,100,246 | 246,246,246 |                       | 4.9672               |
| ![image](https://via.placeholder.com/15/F7F7F7/F7F7F7?) | 21845,100,247 | 247,247,247 |                       | 4.9850               |
| ![image](https://via.placeholder.com/15/F8F8F8/F8F8F8?) | 21845,100,248 | 248,248,248 |                       | 5.0029               |
| ![image](https://via.placeholder.com/15/F9F9F9/F9F9F9?) | 21845,100,249 | 249,249,249 |                       | 5.0207               |
| ![image](https://via.placeholder.com/15/FAFAFA/FAFAFA?) | 21845,100,250 | 250,250,250 | 1061                  | 5.0385               |
| ![image](https://via.placeholder.com/15/FBFBFB/FBFBFB?) | 21845,100,251 | 251,251,251 |                       | 5.0563               |
| ![image](https://via.placeholder.com/15/FCFCFC/FCFCFC?) | 21845,100,252 | 252,252,252 |                       | 5.0741               |
| ![image](https://via.placeholder.com/15/FDFDFD/FDFDFD?) | 21845,100,253 | 253,253,253 |                       | 5.0919               |
| ![image](https://via.placeholder.com/15/FEFEFE/FEFEFE?) | 21845,100,254 | 254,254,254 |                       | 5.1097               |
| ![image](https://via.placeholder.com/15/FFFFFF/FFFFFF?) | 21845,100,255 | 255,255,255 |                       | 5.1276               |