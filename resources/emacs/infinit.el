;; An example of an emacs configuration file for generating some of the
;; coding style elements such as sections and boxes.

;; resection
(global-set-key [(control c)(s)] 'resection-generate)
(load-file "~/.emacs.d/resection.el")

;; rebox
(setq rebox-default-style 223)
(global-set-key [(control c)(b)] 'rebox-comment)
(load-file "~/.emacs.d/rebox.el")

;; indentation
(require 'cc-mode)
(add-to-list 'c-style-alist
  '("infinit"
    (c-basic-offset . 2)
    (c-comment-only-line-offset . 0)
    (c-hanging-braces-alist . ((substatement-open before after)))
    (c-offsets-alist . ((inline-open . 0)
                        (inline-close . 0)
                        (topmost-intro . 0)
                        (comment-intro . 0)
                        (substatement . +)
                        (substatement-open . 0)
                        (case-label . +)
                        (access-label . -)))))
