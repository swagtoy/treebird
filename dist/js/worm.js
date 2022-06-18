// enum Direction
const UP = 0;
const RIGHT = 1;
const DOWN = 2;
const LEFT = 3;

// enum State
const STATE_START = 0;
const STATE_PLAYING = 1;
const STATE_GAME_OVER = 2;
const STATE_GAME_WIN = 3;

let started = false;

class Wormle
{
    constructor(view, title, score, opts)
    {
        this.view = view;
        this.title = title;
        this.score_text = score;

        // State
        this.state = STATE_START;
        
        this.grid_size = opts.grid_size;
        this.view_size = opts.view_size;
        this.tile_size = [ this.view_size[0] / this.grid_size[0],
                           this.view_size[1] / this.grid_size[1] ];

        // Apply class to get properties
        this.view.className = "wormle-view";
        this.apples = [];

        this.state_handle();
        this.loop = null;
    }
    
    reset()
    {
        // Player
        this.body = [];
        this.body_len = 8;
        this.score = 0;
        this.level = 0;
        this.tickspeed = 280;

        // Controls
        this.pos = this.spawn_pos = { x: 3, y: 3 };
        this.direction = RIGHT;
        this.direction_queue = [];
        this.clear_apples();
        this.apples = [];
        this.add_apple();
    }

    clear_apples()
    {
        const len = this.apples.length
        for (let i = 0; i < len; ++i)
        {
            this.apples[this.apples.length-1].elem.remove();
            this.apples.pop();
        }
    }

    obj_rand_position()
    {
        let x = 0, y = 0;
        do
        {
            x = Math.floor(Math.random() * this.grid_size[0]);
            y = Math.floor(Math.random() * this.grid_size[1]);
        } while (this.check_collision(x, y));
        return [x, y];
    }

    create_body(name, x, y)
    {
        let body = document.createElement("div");
        body.className = "wormle-body "+(name ? "wormle-"+name : "");
        body.style.left = x * this.tile_size[0] + "px";
        body.style.top = y * this.tile_size[1] + "px";
        body.style.width = this.tile_size[0] + "px";
        body.style.height = this.tile_size[1] + "px";
        return body;
    }

    worm_grow()
    {
        // If anything else, assume we are starting
        let { x, y } = this.spawn_pos;

        if (this.body.length)
        {
            x = this.body[this.body.length-1].pos.x;
            y = this.body[this.body.length-1].pos.y;
        }

        let body = this.create_body("player", x, y);
        this.view.appendChild(body);

        this.body.push({
            pos: { x, y },
            elem: body,
        });
    }

    add_apple()
    {
        let pos = this.obj_rand_position();

        let body = this.create_body("apple", pos[0], pos[1]);
        this.view.appendChild(body);

        this.apples.push({
            pos: {
                x: pos[0],
                y: pos[1],
            },
            elem: body,
        });
    }

    move(dir)
    {
        const DIR_QUEUE_MAX = 4;
        if (this.direction_queue.length < DIR_QUEUE_MAX)
        {
            this.direction_queue.push(dir);
        }
    }

    last_dir()
    {
        if (this.direction_queue.length)
            return this.direction_queue[this.direction_queue-1];
        else
            return this.direction;
    }

    worm_update()
    {
        // Yeah this is ugly and i don't care its just an easter egg...
        for (let i = this.body.length-1; i >= 1; --i)
        {
            // Shift position forward
            this.body[i].elem.style.left =
                this.body[i-1].pos.x * this.tile_size[0] + "px";
            this.body[i].elem.style.top =
                this.body[i-1].pos.y * this.tile_size[1] + "px";
            
            this.body[i].pos.x = this.body[i-1].pos.x;
            this.body[i].pos.y = this.body[i-1].pos.y;
        }

        this.body[0].pos.x = this.pos.x;
        this.body[0].pos.y = this.pos.y;
        this.body[0].elem.style.left = this.pos.x * this.tile_size[0] + "px";
        this.body[0].elem.style.top = this.pos.y * this.tile_size[1] + "px";

    }

    handle_movement()
    {
        // Update queue
        if (this.direction_queue.length)
        {
            this.direction = this.direction_queue[0];
        }
        this.direction_queue.shift();

        
        // Move in direction
        switch (this.direction)
        {
            case LEFT: {
                this.pos.x--;
                break;
            }
            case RIGHT: {
                this.pos.x++;
                break;
            }
            case UP: {
                this.pos.y--;
                break;
            }
            default: 
            case DOWN: {
                this.pos.y++;
            }
        }
    }

    game_over()
    {
        this.title.innerHTML = `<h1>You lost! Score: ${this.score}</h1>
<p>Press any key...</p>`;
    }

    game_win()
    {
        this.title.innerHTML = `<h1>You won Wormle!</h1>
<p>Pat yourself on the back...</p>
<img src="../img/noname.png"> <img src="../img/noname.png"> <img src="../img/noname.png"> `;
    }

    start()
    {
        for (let i of document.querySelectorAll(".wormle-body"))
        {
            i.remove();
        }

        this.title.innerHTML = `<h1>Wormle</h1>
<input type="button" value="Play" class="wormle-play-btn">`;
        this.reset();

        // Play button
        let btn = this.title.querySelector(".wormle-play-btn");
        let that = this;
        if (btn)
            btn.addEventListener("click", () => {
                that.next_state();
                this.view.focus();
            });
    }

    next_state()
    {
        this.state++;
        if (this.state >= STATE_GAME_OVER)
            this.state = 0;
        this.state_handle();
    }

    update_score(amount = 1)
    {
        this.score += amount;
        this.score_text.innerHTML = this.score;
    }

    state_handle()
    {
        switch (this.state)
        {
            case STATE_START:
            this.start();
            break;

            case STATE_GAME_OVER:
            this.game_over();
            break;

            case STATE_GAME_WIN:
            this.game_win();
            break;

            default:
            this.score_text.innerHTML = this.score;
            this.title.innerHTML = '';
            break;
        }
    }

    get_head()
    {
        let index = this.body.length-1 >= 0 ? this.body.length-1 : 0;
        return this.body[index];
    }

    next_level()
    {
        if (this.level > 6) return;
        this.level++;
        this.tickspeed -= 25;
        if (this.level % 2 == 0) this.add_apple();
    }

    check_collision(x = this.body[0].pos.x, y = this.body[0].pos.y)
    {
        for (let i = 1; i < this.body.length; ++i)
        {
            if (x === this.body[i].pos.x &&
                y === this.body[i].pos.y ||
                x >= this.grid_size[0] || x < 0 ||
                y >= this.grid_size[1] || y < 0)
            {
                return true;
            }
        }
        return false
    }

    update()
    {
        const APPLE_MULTIPLIER = 1;
        if (this.state == STATE_PLAYING)
        {
            if (this.body.length < this.body_len)
                this.worm_grow();

            this.handle_movement();
            this.worm_update();

            // Apple collision
            for (let i = 0; i < this.apples.length; ++i)
            {
                if (this.body[0].pos.x === this.apples[i].pos.x &&
                    this.body[0].pos.y === this.apples[i].pos.y)
                {
                    // Delete apple
                    this.apples[i].elem.remove();
                    this.apples.splice(i, 1);
                    
                    this.update_score();
                    this.body_len += APPLE_MULTIPLIER;
                    this.add_apple();

                    // Check if max size
                    if (this.body_len >= this.grid_size[0] * this.grid_size[1])
                    {
                        this.state = STATE_GAME_WIN;
                        this.state_handle();
                    }

                    if (this.score % 10 === 0)
                    {
                        this.next_level();
                    }
                }
            }
            
            // Game over?
            if (this.check_collision())
            {
                this.state = STATE_GAME_OVER;
                this.state_handle();
            }
        }
    }
}

function start_wormle()
{
    if (started)
        return;
    started = true;
    let wormle_view = document.createElement("div");
    wormle_view.tabIndex = 1;
    let title = document.createElement("span");
    title.className = "wormle-title";
    let score = document.createElement("span");
    score.className = "wormle-score";
    wormle_view.appendChild(score);
    wormle_view.appendChild(title);
    let game = new Wormle(wormle_view, title, score, {
        grid_size: [16, 16], // Tiles
        view_size: [400, 400], // Px
    });

    let callback = () => {
        game.update();
        game.loop = setTimeout(callback, game.tickspeed);
    };
    
    game.loop = setTimeout(callback, game.tickspeed);

    document.addEventListener("keydown", (e) => {
        // Fallback incase
        e = e || window.event;

        if (e.target.classList.contains("wormle-view"))
        {
            if (game.state !== STATE_PLAYING)
                game.next_state();
            else switch (e.keyCode)
            {
                // Left
                case 37: {
                    if (game.last_dir() !== RIGHT) game.move(LEFT);
                    break;
                }
                // Up
                case 38: {
                    if (game.last_dir() !== DOWN) game.move(UP);
                    break;
                }
                // Right
                case 39: {
                    if (game.last_dir() !== LEFT) game.move(RIGHT);
                    break;
                }
                // Down
                case 40: {
                    if (game.last_dir() !== UP)
                        game.move(DOWN);
                    break;
                }
            }
            e.preventDefault();
            return false;
        }
    });
    let view_place = document.querySelector(".about-content");
    document.querySelector(".simple-page").insertBefore(wormle_view, view_place);
}

(function() {
    document.getElementById("about-icon").addEventListener("click", start_wormle);
})();
