# Realm of the Mad Gabe

**Realm of the Mad Gabe** is a C++ video game that I wrote from scratch without the assistance of a game engine or game engine library.

Through this project, I honed my C++ skills and gained hands-on experience with data-oriented design and high-performance computing concepts. I used [GDB](https://sourceware.org/gdb/), [Valgrind](https://valgrind.org/), and [SDL2](https://www.libsdl.org/).

The game is a clone of [Realm of the Mad God](https://en.wikipedia.org/wiki/Realm_of_the_Mad_God), utilizing art from [Oryx Design Lab](https://www.oryxdesignlab.com/).

---

## Demo (Youtube Video)

[![Watch Demo](https://img.youtube.com/vi/aP7Ju_zDels/0.jpg)](https://www.youtube.com/watch?v=aP7Ju_zDels)

---

## Entity Component System (ECS)

The core of this project is an implementation of the [Entity Component System](https://en.wikipedia.org/wiki/Entity_component_system) architectural pattern.

- **Entities** → IDs only  
- **Components** → Pure data  
- **Systems** → Logic operating on components  

This design optimizes CPU memory access and allows the game to sustain high frame rates under load.

---

## Entities and Components

Entities contain no data. Instead, their data lives in separate components stored in contiguous memory pools.

### Entities (ID only)
![Entities](./readmeimages/entity.png)

### Components (data only)
![Components](./readmeimages/componentexamples.png)

---

## Component Storage (Pools)

Pools are wrappers for vectors of components and store them contiguously in memory.

### Pool structure
![Pool](./readmeimages/pool.png)

### Component IDs
![Component ID](./readmeimages/component.png)

### Component templates
![Component Templates](./readmeimages/componenttemplateexample.png)

### All pools
![Pools](./readmeimages/pools.png)

---

## Component Pools Visualization

![Pools Diagram](./readmeimages/pools.drawio.png)

Each pool corresponds to a component type, and each entry stores that component for a specific entity.

---

## Signatures

Component presence is tracked using bitsets.

### Signature (per entity)
![Signature](./readmeimages/signature.png)

### All signatures
![Signatures](./readmeimages/signatures.png)

---

## Adding Components

Example of adding a component to an entity:

![Add Component](./readmeimages/addcomponent.png)

---

## Example Entities With Component Signatures

### Floor
![Floor](./readmeimages/floor.png)
![Floor Signature](./readmeimages/floorsignature.png)

### Tree
![Tree](./readmeimages/tree.png)
![Tree Signature](./readmeimages/treesignature.png)

### Projectile
![Projectile](./readmeimages/projectile.png)
![Projectile Signature](./readmeimages/projectilesignature.png)

Even very different entities can share subsets of components.

---

## Systems

Systems operate on entities that match required component signatures.

### System base class
![System](./readmeimages/system.png)

### Render System (tracks all entities with a sprite and position)
![Render System](./readmeimages/rendersystem.png)

### Signature match example
![Signature Match](./readmeimages/signatureexample1.png)

### Movement System (tracks all entities with position and velocity)
![Movement System](./readmeimages/movementsystem.png)

### Signature mismatch example
![Signature Mismatch](./readmeimages/signatureexample2.png)

Systems that do not share component requirements can often run in parallel.

---

## Performance: Cache Efficiency

Performance comes primarily from **cache efficiency**.

- Components are stored contiguously
- Systems iterate linearly over data
- Cache lines (typically 64 bytes) load multiple components at once
- This leads to high cache hit rates

---

## Data-Oriented Design

Components are kept small and structured around access patterns; Data-oriented design improves performance in ECS systems by minimizing cache misses.

### Object-Oriented Design (counter-example; bad for cache hits)
![OOP](./readmeimages/OOPstats.png)

### Data-Oriented Design (what is used; good for cache hits)
![DOD](./readmeimages/DODstats.png)

---

## Dense Storage

Pools remain dense by filling memory gaps when entities are removed, reducing cache misses:

![Memory Hole](./readmeimages/memoryhole.drawio.png)

---

## AoS vs SoA

The ECS implementation uses an array-of-structures (AoS) layout, where each component is stored as a single object. This is simple and works well when systems need all fields of a component.

Some ECS designs use a struct-of-arrays (SoA) layout instead, where each field is stored in a separate array. This can improve performance by enabling better cache use and vectorization.

AoS is simpler to work with, while SoA can be faster depending on the hardware and access patterns.

Below is a visualization of both layouts for a component with three fields.

### Array of Structures (AoS)
![AoS Code](./readmeimages/aos.png)
![AoS Memory](./readmeimages/aos.drawio.png)

### Structure of Arrays (SoA)
![SoA Code](./readmeimages/soa.png)
![SoA Memory](./readmeimages/soa.drawio.png)

- AoS: better when accessing full components  
- SoA: better for vectorized operations  