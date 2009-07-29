(define make-base-object
  (lambda()
    (lambda(msg)
      (print "Botschaft -->")
      (print  msg)
      (print " wird vom Objekt nicht verstanden!"))))

(define make-animal
  (lambda(aname)
    (define name aname)
    (define base-object (make-base-object))

    (define get-name
      (lambda()
        name))

    (define who-are-you
      (lambda()
        (print "error: method who-are-you not implemented")))

    (define self
      (lambda(msg)
        (if (equal msg 'get-name)
            get-name
            (if (equal msg 'who-are-you)
                who-are-you
                (base-object msg)))))
     self))

(define make-dog
  (lambda(aname)
    (define super (make-animal aname))

    (define who-are-you
      (lambda()
        (print "Wau wau: ")
        (print ((super 'get-name)))))

    (define self
      (lambda(msg)
        (if (equal msg 'who-are-you)
            who-are-you
            (super msg))))
     self))

(define make-cat
  (lambda(aname)
    (define super (make-animal aname))

    (define who-are-you
      (lambda()
        (print "Miau miau: ")
        (print ((super 'get-name)))))

    (define self
      (lambda(msg)
        (if (equal msg 'who-are-you)
            who-are-you
            (super msg))))
     self))

(define make-shelter
  (lambda()
    (define animals nil)
    (define super (make-base-object))

    (define accept
      (lambda(ananimal)
        (define xanimal (locate (lambda(x) (equal ananimal x)) animals))
        (if (equal xanimal false)
            ((lambda()
               (define animals (cons ananimal animals))
               true))
            false)))

    (define dismiss
      (lambda(ananimal)
        (define xanimal (locate (lambda(x) (equal ananimal x)) animals))
        (if (equal xanimal false)
            false
            ((lambda()
               (define animals (remove ananimal animals))
               true)))))

    (define find
      (lambda(aname)
        (define xanimal (locatex (lambda(x) 
                                 (equal aname ((x 'get-name)))) 
                               animals))
        xanimal))

    (define display
      (lambda()
        (for-each (lambda(x) ((x 'who-are-you))) animals)))

    (define self
      (lambda(msg)
        (if (equal msg 'accept)
            accept
            (if (equal msg 'dismiss)
                dismiss
                (if (equal msg 'find)
                    find
                    (if (equal msg 'display)
                        display
                        (super msg)))))))
     self))

(define animal-shelter
  (lambda()
    (define shelter  (make-shelter))
    (define ls       nil)
    (print " ")

    (define bello    (make-dog "bello"))
    (define inka     (make-dog "inka"))
    (define muschi   (make-cat "muschi"))
    (define missi    (make-cat "missi"))
    (print " ")
   
    ((bello  'who-are-you))
    ((muschi 'who-are-you))
    (print " ")

    ((shelter 'accept) bello)
    ((shelter 'accept) inka)
    ((shelter 'accept) muschi)
    ((shelter 'accept) missi)
    (print " ")
                        
    (define found ((shelter 'find) "muschi"))
    (if (equal found false)
        (print "Muschi not found!")
        ((found 'who-are-you)))
    (print " ")

    
    (print "begin of shelter")
    ((shelter 'display))
    (print "end of shelter")
    (print " ")

    ((shelter 'dismiss) missi)
    ((shelter 'dismiss) bello)
    (print " ")

    (print "begin of shelter")
    ((shelter 'display))
    (print "end of shelter")
    (print " ")

    (define found ((shelter 'find) "inka"))
    (if (equal found false)
        (print "Inka not found!")
        ((found 'who-are-you)))
    (print " ")

    (define found ((shelter 'find) "missi"))
    (if (equal found false)
        (print "Missi not found!")
        ((found 'who-are-you)))
    (print " ")

    (print "begin of shelter")
    ((shelter 'display))
    (print "end of shelter")
    (print " ")

    ((muschi 'who-are-you))
    ((shelter 'accept) muschi)
    (print " ")

    (print "begin of shelter")
    ((shelter 'display))
    (print "end of shelter")
    ))
