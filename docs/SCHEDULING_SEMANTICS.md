# Scheduling and blocking-loop limits

`application/foreground.c` retains the complete original caller through typed
external contracts: GPIO entry selection, normal/diagnostic iterations and the
nonreturning reference loop. Offline main never activates that loop.
Preserved pseudocode/instructions remain the reference for blocking menus, threshold
editors, history and charging loops. `application/application.c` keeps these as explicit
resumable states so deterministic host tests can inject input and timer updates.
This is a scheduling transformation, not proof of interrupt/cycle equivalence.

The normal step preserves button dispatch before saved-frame processing, then charging
and deferred battery work. A menu/editor/history/charging step returns without parsing
a measurement. Reception still updates one frame slot and one savedFD-event slot;
a laterFD event replaces the earlier one. There is no inferred queue or lossless log.
After leaving a menu, the pending confirmation/back event retains the recovered value.
A new scheduler test receives twoFD mode events during a menu, advances21 timer
updates, exits and checks that only the latest saved event reaches the normal loop.

A resumable editor completion can yield between its return and the enclosing menu's
next iteration, whereas the original blocking call can continue in the same foreground
invocation. Host arrivals injected at those new yield points may create schedules the
original could not expose at that point. Bounded state/event comparisons and the new
slot test do not remove that difference. Full rendering delays, button sampling and
interrupt arrival timing remain unresolved hardware work.

TIM3 waits21 updates, resets its byte tick counter, schedules battery work and adds20
idle seconds only when auto-off is enabled. It compares strictly greater than the timeout.
The test explicitly enables auto-off when checking that increment. Timer-driven shutdown
and its settings effects remain serialized by test callers. Real ISR/foreground masking,
blocking display work and SPI/FatFs reentrancy have not been validated on hardware.
The optional SDK binding is available but is not initialized by offline main.

FatFs uses one global callback binding and a shared non-reentrant mount object, matching
the observed application organization. Resource workspaces belong to their caller.
Concurrent rendering/filesystem access needs a recovered interrupt exclusion contract;
this continuation does not silently add an RTOS, scheduler or lock policy.

The offline simulator binds the recovered rendering functions and real Flash
resources, with serialized callbacks and an explicitly advanced TIM3 counter.
Display delays advance a virtual elapsed-time counter; they do not invent timer
or UART arrivals. The browser talks to one persistent C process and the HTTP
server handles one request at a time. Its framebuffer is host memory, separate
from the target SRAM/link model. Graph sampling occurs through the recovered
reading path, not a new wall-clock sampler. The optional custom mode restore
sends one command and waits for a supplied ACK; it introduces no ASIC model.

The menu dispatcher now preserves original incremental erases/repaints and cache
hits. Its editor/history waits remain cooperative. A recording-origin menu exit resets
extrema, and a monitor-origin exit resets its own selector/state, as the enclosing
original button handlers do. Default reset returns directly to the outer scheduler
without another menu repaint. These improvements do not establish cycle equivalence.
The C simulator counts unsupported application calls by their original addresses.
The CP936 adapter additionally records the first unmapped ROM read. Neither a
quiet replay nor a passing host test establishes interrupt/panel timing or target
startup readiness.

The explicit diagnostic simulator state replays B56A, performs97f8 debounce/forwarding
and preserves8ed4's first-FD skip and repeated rendering after RX suppression clears.
No new UART interrupt is required for a subsequent diagnostic render. Normal saved-FD
consumption also invalidates the shared label cache as the original does. Both flows
are tested with serialized inputs; UART DMA rearm/copy races remain unvalidated.
The simulator command named `ack` supplies a caller-controlled FD fixture, not evidence
of an actual correlated peer acknowledgement. The independent driver treats send
success solely as local transport acceptance and defines no retry/timeout scheduler.
