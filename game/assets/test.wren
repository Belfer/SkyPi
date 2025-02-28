import "gameobject" for GameObject

class Actor is GameObject {
	construct new() {}

	start() {}
    update() {}
}
GameObject.register(Actor)

class Actor2 is GameObject {
	construct new() {}

	start() {}
    update() {}
}
GameObject.register(Actor2)

class Test {
	construct new() {
		System.print("Construct")
		System.print(GameObject.types.count)
	}

	update() { /*System.print("Update")*/ }
	render() { /*System.print("Render")*/ }
}