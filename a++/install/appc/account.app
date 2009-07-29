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
        (if (equaln msg one) 
            get-balance
            (if (equaln msg two) 
                 deposit
                 (if (equaln msg three) 
                     withdraw
                     (if (equaln msg four) 
                         print-account
                         false)))))) 
    self))

(define konto
  (lambda()
    (define k1 (make-account ten))
    (define k2 (make-account five))
    (define msg-deposit two)
    (define msg-withdraw three)
    (define msg-print four)
    
    ((k1 msg-print))   
    ((k1 msg-deposit) ten)
    ((k1 msg-print))   
    ((k1 msg-withdraw) four)   
    ((k1 msg-print))   

    ((k2 msg-print))   
    ((k2 msg-deposit) nine)   
    ((k2 msg-print))   
    ((k2 msg-withdraw) seven) 
    ((k2 msg-print))))

(konto)
;;;;;                 -->10
;;;;;                    20
;;;;;                    16
;;;;;                    5
;;;;;                    14
;;;;;                    7
