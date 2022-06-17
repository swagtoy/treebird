// enum Direction
const UP = 0;
const RIGHT = 1;
const DOWN = 2;
const LEFT = 3;

// enum State
const STATE_START = 0;
const STATE_PLAYING = 1;
const STATE_GAME_OVER = 2;

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

        this.state_handle();
    }
    
    reset()
    {
        // Player
        this.body = [];
        this.body_len = 8;
        this.score = 0;
        this.food_sum = 4;
        this.level = 0;
        this.tickspeed = 400;

        // Controls
        this.pos = this.spawn_pos = { x: 3, y: 3 };
        this.direction = RIGHT;
        this.direction_queue = [];
        this.apples = [];
        this.add_apple()
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

    start()
    {
        this.title.innerHTML = `<h1>Wormle</h1>
<p>Press any key...</p>`;
        this.reset();

        for (let i of document.querySelectorAll(".wormle-body"))
        {
            i.remove();
        }
    }

    next_state()
    {
        this.state++;
        if (this.state > STATE_GAME_OVER)
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

    check_collision(x = this.body[0].pos.x, y = this.body[0].pos.y)
    {
        for (let i = 1; i < this.body.length; ++i)
        {
            if (x === this.body[i].pos.x &&
                y === this.body[i].pos.y)
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
            for (let apple of this.apples)
            {
                if (this.body[0].pos.x === apple.pos.x &&
                    this.body[0].pos.y === apple.pos.y)
                {
                    apple.elem.remove();
                    this.update_score();
                    this.body_len += APPLE_MULTIPLIER;
                    this.add_apple();
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
    
    setInterval(() => {
        game.update();
    }, game.tickspeed);

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
                    game.move(LEFT);
                    break;
                }
                // Up
                case 38: {
                    game.move(UP);
                    break;
                }
                // Right
                case 39: {
                    game.move(RIGHT);
                    break;
                }
                // Down
                case 40: {
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
