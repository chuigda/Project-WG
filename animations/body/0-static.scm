(pragma 'name "静止状态")
(pragma 'init init-state)
(pragma 'anim animation)

(define init-l1 0)
(define init-r1 0)
(define init-l3 0)
(define init-r3 0)

(define (init-state)
  (set! init-l1 (state 'l 1))
  (set! init-r1 (state 'r 1))
  (set! init-l3 (state 'l 3))
  (set! init-r3 (state 'r 3))
)

(define (animation)
  (if (<= current-frame 25)
    (let (
           [f (/ (float current-frame) 25.0)]
           [l1 (linear-interpolate init-l1 15.0 f)]
           [r1 (linear-interpolate init-r1 15.0 f)]
           [l3 (linear-interpolate init-l3 15.0 f)]
           [r3 (linear-interpolate init-r3 15.0 f)])
      (set-state! 'l 1 l1)
      (set-state! 'r 1 r1)
      (set-state! 'l 3 l3)
      (set-state! 'r 3 r3)
    )
    (raf)
  )
)
