;;;; A++ Interpreter Test
;
(define l1
   (cons three (cons two (cons one nil))))
;;;;;
(ndisp! (length l1))
;;;;;                 --> 3
(ldisp! l1)
;;;;;                 --> 3
;;;;;                     2
;;;;;                     1
;;;;;
(ldisp! (insertion-sort l1))
;;;;;                 --> 1
;;;;;                     2
;;;;;                     3
;
(ndisp! (sum l1))
;;;;;                 --> 6
;
(succ* l1)
(ldisp! (succ* l1))
;;;;;                 --> 4
;;;;;                     3
;;;;;                     2
;
(ldisp! (filter (lambda(x) (gtp x one)) l1))
;;;;;                 --> 3
;;;;;                     2          
;
;;;;
;;;;

(define l1 (cons one (cons three (cons four nil))))

(define l2 (cons seven (cons eight (cons nine (cons ten nil)))))

(ndisp! (faculty four))

(print (fac 5))

(ndisp! (nth two l1))

(for-each ndisp! l2)

(define while-test
  (lambda(n)
    (while (gtp n zero)
      (lambda()
        (ndisp! n)
        (define n (pred n))))))

(while-test ten)


;;;; Objekt-Orientierung in ARS
;;;;
;;;;

(define make-account 
  (lambda(balance)
    (define get-balance
      (lambda()
        balance))

    (define deposit 
      (lambda(amount)
        (define balance (add balance amount))
        balance))

    (define withdraw 
      (lambda(amount)
        (if (gep balance amount)
             ((lambda()
                (define balance (sub balance amount))
                balance))
             false)))

    (define print-account
      (lambda()
        (ndisp! balance)))

    (define self 
      (lambda(msg)
        (if (equal msg 'get-balance) 
             get-balance
             (if (equal msg 'deposit) 
                  deposit
                  (if (equal msg 'withdraw) 
                       withdraw
                       (if (equal msg 'print) 
                            print-account
                            false)))))) 

    self))

(define konto
  (lambda()
    (define k1 (make-account ten))
    (define k2 (make-account five))
    
    ((k1 'print))
    ((k1 'deposit) ten)
    ((k1 'print))
    ((k1 'withdraw) four)   
    ((k1 'print))   

    ((k2 'print))   
    ((k2 'deposit) nine)   
    ((k2 'print))   
    ((k2 'withdraw) seven) 
    ((k2 'print))
   )) 

(konto)


