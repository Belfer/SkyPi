import "gameobject" for GameObject

class Actor is GameObject {
	construct new(base) {
		super(base)
	}

	start() {}
    update() {}
}
GameObject.register(Actor)

class Actor2 is GameObject {
	construct new(base) {
		super(base)
	}

	start() {}
    update() {}
}
GameObject.register(Actor2)

class Test {
	construct new() {
		var actor = GameObject.create(Actor)
	}

	update() { /*System.print("Update")*/ }
	render() { /*System.print("Render")*/ }
}