import "gameobject" for GameObject

class Actor is GameObject {
	construct new(base) {
		super(base)
	}

	start() {
		System.print("Actor.start()")
	}

    update() {
		System.print("Actor.update()")
	}
}
GameObject.register(Actor)

class Actor2 is GameObject {
	construct new(base) {
		super(base)
	}

	start() {
		System.print("Actor2.start()")
	}

    update() {
		System.print("Actor2.update()")
	}
}
GameObject.register(Actor2)

class Test {
	construct new() {
		var actor = GameObject.create(Actor)
		var actor2 = GameObject.create(Actor2)
	}

	update() {}
}