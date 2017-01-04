(load (string-append (ps-path) "/init.scm"))
(load (string-append (ps-path) "/ps.scm"))

; global variables 
(ps-mkvar "env" 0)
(ps-mkvar "tk" 0)

(define noteoff 
 (lambda (t e) 
  (string-append 
   "1 " t " get tog - " e
   " get 0.001 lt * _noteoff fe")))

(define simplefm 
 (lambda (env tick)
  (string-append 
   "tick _offtick fe + " 
   tick 
   " set " 
   tick 
   " get tog 0.1 port " 
   env " set 0 _args tget mtof 0.1 1 1 1 fm "
   env 
   " get * " (noteoff tick env))))

(define square
 (lambda (env tick) 
  (string-append 
   "tick _offtick fe + dup " 
   tick 
   " set tog 0.1 port " 
   env 
   " set 0 _args tget mtof 0.1 0.25 square " 
   env 
   " get * " (noteoff tick env))))

(ps-multi-eval 0 4 (simplefm "_env" "_tk"))
(ps-multi-eval 5 7 (square "_env" "_tk"))

