Nachos 2
========
For the Nachos 2 project the 10 syscalls as defined in syscall.h were defined as follows:

##### Syscalls
[SC\_Exit](#sc_exit)  
[SC\_Exec](#sc_exec)  
[SC\_Join](#sc_join)  
[SC\_Create](#sc_create)  
[SC\_Open](#sc_open)  
[SC\_Read](#sc_read)  
[SC\_Write](#sc_write)  
[SC\_Close](#sc_close)  
[SC\_Fork](#sc_fork)  
[SC\_Dup](#sc_dup)  


<a name="sc_exit"/>
SC_Exit
--------

- [x]    Check for players nearby (if werewolf)
- [x]    Restart game (if admin)
- [x]    Get votable players
- [x]    Place a vote on a person (if it is day)
- [x]    Kill a player (if werewolf and night)
- [x]    Report current position
- [x]    Get score list (only viewable by users)
- [x]    Kill players that do not update at least every 5 minutes in a game
Special Features
----------------

- [x]    Web Console for Admin (in progress)
- [x]   Online interface for users to be added
- []    Add infection rate to werewolves, allowing for a chance to infect
- [x]    Add web interface for users(in progress)
- []    Allow for users to be set as Admins
- [x]    Allow for more new game settings, setting a kill zone/scent zone (in progess)
- []    SSL

The currently viewable links include:
<table>
  <tr>
    <th>Link</th><th>Description</th>
  </tr>
  <tr>
    <td>\</td><td>Shows a form to add a user.</td>
  </tr>
  <tr>
    <td>\player</td><td>prompts for login by a user, also shows forms for allowing the user to participate in a game(assuming a game is running and the user is currently linked with a player in the game).  Forms include a selectable vote list, and a scent list which werewolves can submit on to attack people.</td>
  </tr>
    <tr>
    <td>\admin</td><td>accessible only by the super admin(for now), allows for the creation of a new game and the addition of users</td>
  </tr>
</table>

Post
----
<table>
  <tr>
    <th>Link    </th><th>Description</th>
  </tr>
  <tr>
    <td>\newgame</td><td>starts a new game.  Must have admin credentials to do this.</td>
  </tr>
  <tr>
    <td>\players\location</td><td>updates a player's location based on the post.  Requested params are both of the type double and must be passed in as 'lat' and 'lng'</td>
  </tr>
    <tr>
    <td>\players\kill</td><td>allows a werewolf to kill its specified victim.  Can only occur at night.  Requested param is of type string and must be passed in as 'victim'.</td>
  </tr>
    <tr>
    <td>\players\vote</td><td>allows a player to vote during the daytime.  Requested param is of type string and must be passed in as 'voted'.</td>
  </tr>
</table>



Get(without .jsp views)
-----------------------
<table>
  <tr>
    <th>Link    </th><th>Description</th>
  </tr>
  <tr>
    <td>\game</td><td>returns a boolean value of whether the game is running or not to the requester.</td>
  </tr>
  <tr>
    <td>\highscores</td><td>returns a list of WerewolfUser's scores</td>
  </tr>
    <tr>
    <td>\players\getVotable</td><td>returns a list of players that can be voted for</td>
  </tr>
    <tr>
    <td>\players\all</td><td>returns a list of all Players.  If a game is running the list is sanitized.</td>
  </tr>
      <tr>
    <td>\players\scent</td><td>allows a werewolf to 'scent' the people nearby it.  Returns a list of players where players that are in scent but not kill range have a score of 0, those in kill range have a score of 1, and werewolves in range have a score of 2.</td>
  </tr>
        <tr>
    <td>\players\alive</td><td>returns a list of currently living players.  List is sanitized.</td>
  </tr>
  <tr>
    <td>\logout</td><td>when using a web interface allows users to logout assuming they have cleared their cache since their last page access.  Will be fixed in later implementations or removed.</td>
  </tr>
</table>

Testing
=======
Currently there is only one large python script for testing purposes.  It prints out statements telling the user what it is attempting to do.  It then plays through a game and checks at the end to ensure that a game is no longer running.
To run the script go under the director scritps and run newGame.py

To use app go to: http://secure-lake-6285.herokuapp.com
