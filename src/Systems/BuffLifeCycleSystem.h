// TODO
// This system will manage the lifecycle of player/monster buffs 
// make new component, BuffComponent 
// each buff/debuff has 2 data: 
// 1) endTick = tick when buff should be set to false
// 2) bool buff = true or false representing whether or not a buff is on
// Update() = if SDL_Getticks() >= endtick; turn off buff
// buffs will be turned on via events
// movementSystem-> if collided with a trap 
// KBMS-> if pressing space for characters like warrior