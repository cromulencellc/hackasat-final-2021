# HaS2-scoreboard


This is a visual web front end for the Hack-a-Sat 2 scoreboard.  It uses bootstrap for layout and grids as well as a DOM addressable inline SVG of the Cromulence FLATSAT cad drawing.  Download all the files and extract into /var/www.  Browse to https://<server origin>/version3/index.html.   It's been tested with Apache.
  
get-scores2.py will replay the scores every second.   There is an animation of the solar panels which will pivot around the spacecraft when the score changes.  You can install it as a cron job to provide constant replay.


