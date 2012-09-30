(defun resection-generate ()
   (interactive)
   (let ((section-name ""))
    (setq section-name (read-from-minibuffer
                        (format "section name: ")
                       )
    )
    (insert-string "//")
    (newline)
    (insert-string (concat
                    "//"
                    " ---------- "
		    section-name
		    " "
                    (make-string
		     (- 79 (+ (length "//")
			      (+ (length " ---------- ")
				 (+ (length section-name)
				    (length " ")
				 )
			      )
			   )
		     )
		     ?-)
                   )
    )
    (newline)
    (insert-string "//")
    (newline)
   )
)
