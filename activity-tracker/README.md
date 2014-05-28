This is the preliminary work on a Pebble activity tracker that provides a measure of whether accelerometer samples taken over the course of each second constitute either high, low, or no activity.

High and low activity intervals are calculated the same way as the Funf open sensing library's ActivityProbe, and is intended to be integrated with that system as an alternative to that probe. In its current state, this pebble app simply shows the current level of activity taken over the most recent second, but does not perform the thresholding or sync this value to the phone. 
