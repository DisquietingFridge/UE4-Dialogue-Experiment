# Stephen

This is an experimental learning project currently on hiatus. I think it'd be interesting to pick back up and see how I could improve upon it though, since I've learned a lot about software design practices since I started it.

In terms of learning outcomes, the goals of this project were to:

* Get to grips with implementing CPP code within Unreal Engine 4. I was familiar with the language in general (especially in terms of smaller-scope things like algorithm-puzzles) but still intimidated at the idea of using it within UE4's existing codebase.

* Figure out how to use UE4's UI widgets.

In the level, the player can walk up to prolific horror-author Stephen King and press "E", which will allow him to complain that his publisher wants him to stop writing about psychic guys and cornfields.

<img src="https://user-images.githubusercontent.com/30654622/219537175-0611992e-99c5-49cd-9bf5-e0cacee73532.mp4" width="1%" height="1%">




Aside from just presenting some text in a box, I wanted the presentation to feel recognizably "game-y", which meant

* The UI window is populated by the text-block one letter at a time
* The user can press "E" while the window is being populated to force all the text to appear
* Once the window as been fully populated, the user presses "E" to move onto the next text block (or end the dialogue, if it's finished).

Since one block of text is followed by another, this was a good opportunity to practice data-driven design. The dialogue is defined in a CSV table that the program imports and processes, and each block points to another entry that will follow it.

Of course, it would be unusual for a player-character to press the interaction button from just anywhere and talk to someone, so I also designed a rudimentary interaction system. The player has a cone-like collision object attached to them, and it uses a custom collision channel. In this system, interactable objects also possess colliders on this channel. 

When the player's collider is overlapping with an interactable object's, a reference to that object it is kept by the player. When the interact input is received, a signal is sent by the player to the object. Thus, if there's no overlap (the player is far away from King), there's no interaction. I set up the system anticipating the need for more sophistication later (e.g. multiple interaction candidates in range of the player), but it's currently pretty simple.

Were I to revisit this project, I think I'd overhaul the dialogue system's architecture to be less implementation-specific. I had also planned to make the system more interactive, i.e. allowing the player to select dialogue options. At the time I felt paralyzed in terms of committing to a design for this: questions like how the dialogue should be presented, whether options should be highlighted in a mouse-over, etc. 

But having learned more principles of software architecture, these are problems of specific implementation, which wouldn't prevent me from creating an underlying dialogue system. I would design by contract (e.g. the system outputs player-dialogue options, receives a selection, moves through the dialogue table as appropriate), and specific UI implementations wouldn't constrain the codebase.

