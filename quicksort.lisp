(define length_ 
  (lambda (ls n)
    (if (null? ls)
	n
      (length_ (cdr ls) (+ n 1)))))

(define length 
  (lambda (ls)
    (length_ ls 0)))


(define iter
  (lambda (ls p l1 l2)
    (if (null? ls)
	(cons l1 l2)
      (if (> (car ls) p)
	  (iter (cdr ls) p (cons (car ls) l1) l2)
	(iter (cdr ls) p l1 (cons (car ls) l2))))))

(define qs 
  (lambda (ls)
    (if (> (length ls) 1)
	(append (qs (cdr (iter ls (car ls) nil nil)))  (qs (car (iter ls (car ls) nil nil))))
      ls)))

(qs (list 23 8 7 2 9))
	