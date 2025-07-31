from pylnsocket import LNSocket

try:
    lns = LNSocket("[NODE_ID]", "[HOST]:[LNSOCKET_PORT]")
    ret = lns.Call(bytes(b"[RUNE]"), "getinfo")
    print("Response:\n", ret)

except Exception as exc:
    print("Exception: ", exc)
