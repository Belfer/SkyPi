import "gameobject" for GameObject

class Actor is GameObject {
	construct new() {}

	start() {}
    update() {}
}
GameObject.register("test", "Actor")

class Actor2 is GameObject {
	construct new() {}

	start() {}
    update() {}
}
GameObject.register("test", "Actor2")

#!key
#ignored //compiled out
#!group(key=value, key=32, key=false)
class Example {
	#!getter
	getter {}

	// { regular(_,_): { null: { regular:[null] } } }
	#!regular
	regular(arg0, arg1) {}

	// { static other(): { null: { isStatic:[true] } } }
	#!isStatic = true
	static other() {}

	// { foreign static example(): { null: { isForeignStatic:[32] } } }
	#!isForeignStatic=32
	static example() {}
}

class Test {
	construct new() {
		System.print("Construct")
		System.print(GameObject.types.count)
		for (i in GameObject.types) {
			System.print("classModule: %(i.classModule), className: %(i.className)")
		}
		
		System.print("self attributes: %(Example.attributes.self.count)")
		for (i in Example.attributes.self) {
			System.print("	group: %(i.key)")

			for (j in i.value) {
				System.print("		key: %(j.key), value: %(j.value)")
			}
		}

		System.print("method attributes: %(Example.attributes.methods.count)")
		for (i in Example.attributes.methods) {
			System.print("	method: %(i.key)")
		
			for (j in i.value) {
				System.print("		group: %(j.key)")

				for (k in j.value) {
					System.print("			key: %(k.key), value: %(k.value)")
				}
			}
		}
	}

	update() { /*System.print("Update")*/ }
	render() { /*System.print("Render")*/ }
}