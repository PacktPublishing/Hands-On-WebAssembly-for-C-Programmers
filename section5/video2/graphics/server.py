from operator import itemgetter
from bottle import get, post, request, run, static_file

leaderboard = {}

@post("/gameScore/<name>")
def get_game_score(name):
    left,right = request.body.read().decode("utf-8").split()
    leaderboard[name] = (int(right)/(int(left)+int(right)) if int(right) else 0)

@get("/leaderboard")
def get_leaderboard():
    return ("Name Winning %<br />" + 
            "<br />".join( f"{name}: {pct*100}%" 
                for name, pct in sorted(leaderboard.items(), key=itemgetter(1))[::-1]))

@get("/")
def index_file():
    return send_static("/pong.html")

@get("/<path:path>")
def send_static(path):
    return static_file(path, root=".")

run(host="0.0.0.0", port=8000)
