### L System Renderer
 
A terminal-based [L-system](https://en.wikipedia.org/wiki/L-system) renderer written in C. Generates fractals and prints them to the terminal using Bresenham's line algorithm.
  
## Building
 
Requires GCC and GNU Make.
 
```sh
make
```
 
### Nix
 
A Nix flake is included for the dev env:
 
```sh
nix develop
make
```
 
## Running
 
```sh
./bin/main
```
 
Press `q` to quit.
 
## Configuration
 
Edit `src/main.c` to change the L-system. The relevant call in `main()` is:
 
```c
char *lstring = rewrite("F", "F", "F-F++F-F-----++FF+-F", 5);
render_with_branches(lstring, 5, 60);
```
 
| Parameter | Description |
|-----------|-------------|
| Axiom (`"F"`) | The starting string |
| Rule find (`"F"`) | The character to replace on each iteration |
| Rule replace (`"F-F++F-F..."`) | What to replace it with |
| Iterations (`5`) | How many rewrites to do |
| Step size (`5`) | Distance moved per `F`|
| Turn angle (`60`) | Degrees turned |
 
### Example L-systems
 
**Koch snowflake** (angle 60°):
```
Axiom: F
Rule:  F → F-F++F-F
```
## Project structure
 
```
.
├── src/
│   └── main.c       # Source code
├── bin/             # Binary
├── Makefile
├── flake.nix        # Nix flake
└── flake.lock
```

## Todo

- Multiple rewrite rules
- Branch support
- Colour support
- Live keyboard parameter changing (iterations, step size, angle)
- Pan and zoom navigation
- Named fractal presets switchable by keypress
- Iteration animation (watch fractal grow step by step)
- Config file
- Export to text file

## License
 
Unlicensed / public domain — do whatever you like with it
