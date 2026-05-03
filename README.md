# Realm of the Mad Gabe

**Realm of the Mad Gabe** is a C++ video game made without the assistance of a game engine or game engine library. It was made using [GDB](https://sourceware.org/gdb/), [Valgrind](https://valgrind.org/), and [SDL2](https://www.libsdl.org/). It is a clone of [Realm of the Mad God](https://en.wikipedia.org/wiki/Realm_of_the_Mad_God), utilizing art from [Oryx Design Lab](https://www.oryxdesignlab.com/).

---

## Demo (Youtube Video)

[![Watch Demo](https://img.youtube.com/vi/aP7Ju_zDels/0.jpg)](https://www.youtube.com/watch?v=aP7Ju_zDels)

---

## Entity Component System (ECS)

The core of this project is an implementation of the [Entity Component System](https://en.wikipedia.org/wiki/Entity_component_system) architectural pattern.

- **Entities** → IDs only  
- **Components** → Pure data  
- **Systems** → Update components  

This design is CPU cache-friendly, resulting in high performance and high frame rates.

---

## Entities and Components

Entities contain no data. Instead, their data lives in separate components.

### Entities (ID only)
![Entities](./readmeimages/entity.png)

### Components (data only)
![Components](./readmeimages/componentexamples.png)

---

## Component Storage (Pools)

Entities do not store their own components. Instead, they are stored in pools which are arrays of components.

### Pool Implementation
![Pool](./readmeimages/pool.png)

### Components Full Implementation

Components use static per-type ID generation via a templated base class.

![Component ID](./readmeimages/component.png)

![Component Templates](./readmeimages/componenttemplateexample.png)

### Pool Storage

All pools are stored in an array, allowing a component ID to be used to access its corresponding pool, where each entry stores the component for a specific entity.

![Pools](./readmeimages/pools.png)

![Pools Diagram](./readmeimages/pools.drawio.png)

---

## Signatures

The total set of components that an entity has is tracked using a bitsets which is indexable by component ID.

![Signature](./readmeimages/signature.png)

![Signatures](./readmeimages/signatures.png)

---


## Example Entities With Component Signatures

### Floor
![Floor](./readmeimages/floor.png)
![Floor Signature](./readmeimages/floorsignature.png)

A floor has a sprite and position component.

### Tree
![Tree](./readmeimages/tree.png)
![Tree Signature](./readmeimages/treesignature.png)

A tree has a sprite, position, hitbox, and rigidbody component.

### Projectile
![Projectile](./readmeimages/projectile.png)
![Projectile Signature](./readmeimages/projectilesignature.png)

A projectile has a sprite, position, hitbox, velocity, etc. 

---

## Putting it all together: Adding a component to an entity

To add a component to an entity:

- The component ID is used to index the array of pools to get the pool for that component type
- The component is inserted into the pool 
- The pool associates the stored component position with the entity’s ID
- The signature of the entity is updated to indicate that it has this component
- Relevant systems are updated to track this entity if it matches their requirements (see next section!)

![Add Component](./readmeimages/addcomponent.png)

---

## Systems

Systems perform updates on entities by modifying their components and are specialized to track only those entities whose component signatures match their own.

### System base class
![System](./readmeimages/system.png)

### Example: Render System (tracks all entities with a sprite and position)
![Render System](./readmeimages/rendersystem.png)

### Signature match example
![Signature Match](./readmeimages/signatureexample1.png)

### Example: Movement System (tracks all entities with position and velocity)
![Movement System](./readmeimages/movementsystem.png)

### Signature mismatch example
![Signature Mismatch](./readmeimages/signatureexample2.png)

---

## Performance: Cache Efficiency

Performance comes primarily from **cache efficiency**.

- Components are stored contiguously
- Systems iterate linearly over data
- Cache lines (typically 64 bytes) load multiple components at once
- This leads to high cache hit rates

---

## Data-Oriented Design

Components are kept small and structured around access patterns (fields are added to components with system usage in mind); Data-oriented design improves performance in ECS systems by minimizing cache misses.

### Object-Oriented Design (counter-example; bad for cache hits)
![OOP](./readmeimages/OOPstats.png)

### Data-Oriented Design (what is used; good for cache hits)
![DOD](./readmeimages/DODstats.png)

---

## Dense Pool Storage

Pools remain dense by filling memory gaps when entities are removed, reducing cache misses:

![Memory Hole](./readmeimages/memoryhole.drawio.png)

---

## AoS vs SoA

This ECS implementation uses an array-of-structures (AoS) layout, where each component is stored as a single object. This is simple and works well when systems need all fields of a component. Some ECS designs use a struct-of-arrays (SoA) layout instead, where each field is stored in a separate array. This can improve performance by enabling better cache use and vectorization.

AoS is simpler to work with, while SoA can be faster depending on the hardware and access patterns. Below is a visualization of both layouts for a component with three fields:

### Array of Structures (AoS; what was used)
![AoS Code](./readmeimages/aos.png)
![AoS Memory](./readmeimages/aos.drawio.png)

### Structure of Arrays (SoA; alternative design)
![SoA Code](./readmeimages/soa.png)
![SoA Memory](./readmeimages/soa.drawio.png)

- AoS: better when accessing full components  
- SoA: better for vectorized operations  

---

## Manager Class

Essential operations such as the management of entities, pools, and systems are the responsibility of an ECS "Manager" class which I've omitted to keep explanations concise and centered on the essential underlying mechanisms.