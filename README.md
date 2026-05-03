# Realm of the Mad Gabe

**Realm of the Mad Gabe** ([GitHub](https://github.com/GabrielWelvaert/Realm-of-the-Mad-Gabe)) is a C++ video game that I wrote from scratch without the assistance of a game engine or game engine library.

Through this project, I honed my C++ skills and gained hands-on experience with data-oriented design and high-performance computing concepts. I used [GDB](https://sourceware.org/gdb/), [Valgrind](https://valgrind.org/), and [SDL](https://www.libsdl.org/) for hardware interactions.

The game is a clone of [Realm of the Mad God](https://en.wikipedia.org/wiki/Realm_of_the_Mad_God), utilizing art from [Oryx Design Lab](https://www.oryxdesignlab.com/).

---

## Demo

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

## Example Entities

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

### Render System
![Render System](./readmeimages/rendersystem.png)

### Signature match example
![Signature Match](./readmeimages/signatureexample1.png)

### Movement System
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

Components are kept small and structured around access patterns.

### Object-Oriented Design
![OOP](./readmeimages/OOPstats.png)

### Data-Oriented Design
![DOD](./readmeimages/DODstats.png)

Data-oriented design improves performance in ECS systems by minimizing cache misses.

---

## Dense Storage

Pools remain dense by filling memory gaps when entities are removed:

![Memory Hole](./readmeimages/memoryhole.drawio.png)

---

## AoS vs SoA

### Array of Structures (AoS)
![AoS Code](./readmeimages/aos.png)
![AoS Memory](./readmeimages/aos.drawio.png)

### Structure of Arrays (SoA)
![SoA Code](./readmeimages/soa.png)
![SoA Memory](./readmeimages/soa.drawio.png)

- AoS: better when accessing full components  
- SoA: better for vectorized operations  

---

## Summary

This project explores:

- Custom ECS implementation
- Cache-efficient data layouts
- High-performance C++ design
- Tradeoffs between AoS and SoA

It was a rewarding mix of systems design, performance engineering, and low-level programming.