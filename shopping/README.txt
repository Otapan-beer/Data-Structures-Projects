Name: Otakar Lichnovsky
Time: I don't want to talk about it... (she was 18, I swear officer)

  WELL the reason why I stated with the TikTok homework today is simple because I started two days ago and I slept about 15 hours split across 4 days. Not good.
Now you are probably like "what the hell is this guy yapping about". Well, I started homework 9... shopping. I have spent 8 hours on the wrong shit and was like "why do I keep getting child excited with status 0 on submitty" until I realized, YAY!

  BEFORE YOU CONTINUE: let me clarify one thing, my comments HAVE A SYSTEM, unlike the someone who took 2 points of me thinks:

  If it is like this: 

...
//comment
code;
code;
...

  It means that the comment applies to a chunk of code (or sometimes until next comment)

  If it is like this:

...
code; //comment
code;
...

  then I am commenting just that specific one line

AND if it is like this:

...
//comment
code; //comment
code; 
...

  then I am commenting the whole block and clarifying on that one single line... or I am just yapping to myself because yall probably don't even read it

  So with this quick "comment sesh with Ota" I hope I made myself clear.


  Anyways, back to the story. After I have realized that I am a prime candidate for dumb (I was asking my frineds questions, they were looking at me like I was from a different planet for a reason lol), I have hopped on the *correct* homework 9 and did some quick v1 of about 200 lines in a single file that took me about 2-3 hours to setup and I just figured I would try and get the output as right as possible when it comes to spacing so I do not have to worry about it later. After that, the versions 2 to 5 were mostly usuccessful attempts for trying to order the videos correctly (I got it on version 6) and then it was just below 4 hours of trying to figure out a question of why are my used and views lower than they should be. It was surprisingly painless, but after missing only about 0-10% (I got 18 points because music was right, turns out the issue was that by using unordered set I was not counting the duplicate views and uses of the false positives, so I just used vector instead, it might be a couple of pointns minus but whatever), I asked chat to help me with finding the reason why and the problem and surprisingly (probably because I got the premium and it is not dumb after 3 uses because it lowers the models over time on free) it got it to the point where I was having just one test case wrong (I think v11 or v12), where in test case 5.1 this: 

========================
#newyork
used 88 times
417592200 views

cover url: https://p16-sign.tiktokcdn-us.com/obj/tos-useast5-p-0068-tx/c3fd887eebd64641953a0d98fdbcae70?x-expires=1700452800&x-signature=H%2FalVtW5d%2FSwmlpXE%2B3q4It4bjM%3D
web video url: https://www.tiktok.com/@caerabooo/video/7242507137543425323
cover url: https://p16-sign-useast2a.tiktokcdn.com/obj/tos-useast2a-p-0037-euttp/oIbM97K3KJ6GIgDTs0reRIfnE7CC1QjUfMeAFG?x-expires=1700452800&x-signature=IndBdLnhx05kmP13bMQIRPLIY5g%3D
web video url: https://www.tiktok.com/@emanuelabarretta/video/7271721632329239841
cover url: https://p16-sign-va.tiktokcdn.com/obj/tos-maliva-p-0068/79ae4d425f0a4ae8b8d7a5e0f3f5b589_1650391089?x-expires=1700452800&x-signature=%2Bq7IPVVSaIrSJwmsewqaRqy36EU%3D
web video url: https://www.tiktok.com/@ballsco/video/7088375668114836742
========================

was supposed to look like this: 

========================
#newyork
used 88 times
417592200 views

cover url: https://p16-sign.tiktokcdn-us.com/obj/tos-useast5-p-0068-tx/c3fd887eebd64641953a0d98fdbcae70?x-expires=1700452800&x-signature=H%2FalVtW5d%2FSwmlpXE%2B3q4It4bjM%3D
web video url: https://www.tiktok.com/@caerabooo/video/7242507137543425323
cover url: https://p16-sign-va.tiktokcdn.com/obj/tos-maliva-p-0068/79ae4d425f0a4ae8b8d7a5e0f3f5b589_1650391089?x-expires=1700452800&x-signature=%2Bq7IPVVSaIrSJwmsewqaRqy36EU%3D
web video url: https://www.tiktok.com/@ballsco/video/7088375668114836742
cover url: https://p16-sign-useast2a.tiktokcdn.com/obj/tos-useast2a-p-0037-euttp/oIbM97K3KJ6GIgDTs0reRIfnE7CC1QjUfMeAFG?x-expires=1700452800&x-signature=IndBdLnhx05kmP13bMQIRPLIY5g%3D
web video url: https://www.tiktok.com/@emanuelabarretta/video/7271721632329239841
========================

  So that was another about 3 hours of time wasted trying to figure out just this one thing. In the end I told Chat to assume that the homework description I posted out of desperation along with like 500 console debug lines might be wrong and to try and find the reason. I still do not fully grasp why was that error occurring only in that test case, but it came up with the best sorting option after a couple tries and bunch of console debugging it derived the best method to sort while also not violating the test cases I fed to it (apparently it is just "sort by the video string comparison based by alphabet position"). That is why the line 186 and line 211 in the structv7 are so weird (and yes, the version number 7 is high like that because of this little detail).
  To top it off, I also had my revenge h file take 4 tries to perfect (time well spent).
  So in total, I have spent about 10 hours purely on this assignment assuming I count in the writing of the README.

  Time to sleep!
