extensions [matrix]

globals [
  divisions      ;the factor by which the length of the fractal's line segments decreases (M in the Hausdorff dimension equation)
  fractal-dim    ;Hausdorff dimension of the fractal
  initial-length ;initial length of the line segments
  len            ;current length of the line segments (after iteration)
  old-box-count  ;number of red boxes covering parts of the fractal at the previous step
  new-box-count  ;current number of boxes (red) covering parts of the fractal
  x-axis-list    ;list of all the x-values for the log-log box-counting dimension plot
  y-axis-list    ;list of all the y-values for the log-log box-counting dimension plot
  box-size       ;current size of boxes
  explore?       ;whether or not the boxes need to move and find a portion of the fractal to cover
  iteration      ;counts the box-counting iteration
  iterations-to-go
  slope          ;slope of the box-counting log-log plot regression line, equal to the box-counting dimension
  r-square       ;r-squared value for the box-counting log-log plot regression line
  lin-reg-eq     ;linear regression equation for the log-log plot
  any-new-boxes?
  x-cor           ;initial x-coordinate for the first turtle
  y-cor            ;initial y-coordinate for the first turtle
  fractal-example ;the name of the fractal being drawn
]



;globals
; [divisions fractal-dim initial-length
;  len walk-count old-box-count
;  new-box-count x-axis-list y-axis-list
;  box-size explore? iteration
;  iterations-to-go slope lin-reg-eq
;  any-new-boxes? x-cor y-cor
;  ]

turtles-own   ;used only for making fractal examples and custom frctals if you choose.
  [new?]

breed         ;used only for taking box counting dimension of fractal examples.
 [boxes box]

boxes-own
  [past? present? future?]

patches-own [fractal?]

to startup
   setup
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;; Fractal Procedures ;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;


;Clears the world and sets up an initial turtle
to setup
  ;Initializing values
  set explore? false
  set x-axis-list [ ]
  set y-axis-list [ ]
  set fractal-dim "N/A" ;the first iteration has not yet been drawn, so we don't know what the Hausdorff dimension is

  ;Setting the initial turtle position
  if (fractal-example = "koch-curve" or fractal-example = "cantor-set") [set x-cor -150 set y-cor 0 set divisions 3]
  if(fractal-example = "levy-curve" or fractal-example = "dragon-curve") [set x-cor -60 set y-cor 0 set divisions sqrt 2]
  if(fractal-example = "sierpinski-triangle")[set x-cor -100 set y-cor 0]
  crt 1 ;creating the initial turtle
  ask turtles [
    ;initializing turtle properties
    set new? false
    set shape "circle"
    set size 1
    set color green
    setxy x-cor y-cor
    set heading 90
    ifelse(fractal-example != "tree")
    [
      set initial-length 100
      pd
      fd initial-length * divisions
      set len initial-length
      pu
      setxy x-cor y-cor
    ]
    [
      set divisions 2
      set initial-length 100
      set x-cor 0 set y-cor -150
      set len initial-length
      setxy x-cor y-cor
      set heading 0
      walk walk
    ]
  ]
  reset-ticks
  if fractal-example = "sierpinski-triangle" [iterate]
end

;Depending on the example chosen for the fractal, this will iterate the fractal
to iterate

  ;The tree fractal requires that the previous iterations remain on the screen
  if fractal-example != "tree" [clear-drawing]

  ;Creates standard Koch curve
  if fractal-example = "koch-curve"
  [
    ask patches [set pcolor black] ; clears screen
    ask turtles [
      set new? false ]
    t  walk l 90 t  walk r 90 t  walk r 90 t  walk l 90 t walk d ;l-system
    set divisions 3 ;the factor by which the length decreases
    set len (len / divisions)
    fractal-dimension  ;calculates & updates fracal dimension
    tick
  ]

  ;Creates same fractal tree as in simple fractals and the L-systems
  if fractal-example = "tree"
  [
    ;ask patches [set pcolor black]
    ask turtles[
      set new? false]
    r 15 walk  t r 180 skip len r 180 l 40 walk t
    ask turtles with [new? = false] [die]
    set divisions 2
    set len (len / divisions)
    fractal-dimension
    tick
  ]

  ;Creates the Levy C Curve
  if fractal-example = "levy-curve"
  [
    ask patches [set pcolor black]
    ask turtles[ set new? false]
    l 45 t walk r 90 t walk d
    set divisions sqrt 2
    set len (len / divisions)
    fractal-dimension
    tick
  ]


  ;Creates the Sierpinski triangle
  if fractal-example = "sierpinski-triangle"
  [
    ask patches [set pcolor black] ; clears screen
    ask turtles [set new? false pd]
    ;set len len / 2
    t walk t walk l 120 walk walk l 120 walk l 120 t r 120 walk d
    set divisions 2
    set len (len / divisions)
    fractal-dimension
    tick
  ]

  ;Creates the Cantor set
  if fractal-example = "cantor-set"
  [
    ask patches [set pcolor black] ; clears screen
    ask turtles [set new? false ]
    t walk
    skip len
    t walk
    d
    set divisions 3
    set len (len / divisions)
    fractal-dimension
    tick
  ]

  ;Creates the Heighway dragon curve
  if fractal-example = "dragon-curve"
  [
    ask patches [set pcolor black] ; clears screen
    ask turtles [
      set new? false ]
    r 45 t walk l 90  walk l 180 t d
    set divisions  sqrt 2
    set len (len / divisions)
    fractal-dimension
    tick
  ]
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;Box-Counting-Dim;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

to box-counting-setup
    set box-size initial-box-length
    set iteration iteration
    make-initial-box
end

to box-counting-go
   ask boxes ; clears screen in case there are any old boxes
   [die]
   set initial-length 100

    if box-size >= initial-length ;prevents boxes from getting too big for the world
     [stop]
    set box-size box-size + increment
    set iteration iteration + 1
    set iterations-to-go 91 - iteration
    set old-box-count 0
    set new-box-count 1    ;eliminates an error for first round
    make-initial-box
    make-neighbors-count-patches

end

 ;makes a starter box at the beginning of each run with present? = true.
 ;This box will then be used to make boxes with future? = true
 ;which will be used for the next run.
to make-initial-box
    create-boxes 1
  ask boxes [
    set shape "square"
    set size box-size
    setxy  x-cor y-cor
    set heading 90
    set color red
    set past? false
    set present? true
    set future? false
    ]
end

 ;makes a Moore neighborhood around the starter box and counts patches below each new box (exploit).
 ;If there are no new boxes with patches under them for a given run a box is sent outside the neighborhhod
 ;to cover non-contiguous patches (explore). If this box finds no new patches the run is complete.

to make-neighbors-count-patches
    ask boxes with [present? = true ]
    [make-neighbors]

    ask boxes with [future?  = true]
       [exploit]
       count-new-boxes
      if any-new-boxes?     = false
       [explore]

      if any-new-boxes? = false
       [calculate-box-counting-dimension
         stop]

      update-past-present-future-states
      tick
      if any-new-boxes? = true
      [make-neighbors-count-patches]
end

to make-neighbors
     hatch 1 [fd box-size rt 90
      set present? false set future? true
      hatch 1 [fd box-size rt 90
      set present? false set future? true
      hatch 1 [fd box-size
      set present? false set future? true
      hatch 1 [fd box-size rt 90
      set present? false set future? true
      hatch 1 [fd box-size
      set present? false set future? true
      hatch 1 [fd box-size rt 90
      set present? false set future? true
      hatch 1 [fd box-size
      set present? false set future? true
      hatch 1 [fd box-size
      set present? false set future? true
       ]]]]]]]]
end

to exploit
   if count boxes in-radius (box-size / 2) > 1  ; eliminates duplicates
   [die]

   if count patches-under-me = 0
   [ die ]
end

to-report patches-under-me
     report  patches in-radius  ( (1.4 * size ) / 2 )  with [pcolor = green]
end

to explore
 if count boxes with [present? = true] > 0 [
   ask patches with [pcolor = green] [
 ifelse count boxes in-radius  (  box-size ) = 0
  [set explore? true]
  [stop]
  ]
 ]

 if explore? [
 ask one-of boxes with [present? = true] [
  hatch 1 [
  set present? false set future? true
  move-to min-one-of patches with [pcolor = green and count boxes in-radius  ( box-size ) = 0 ]
  [distance myself]]
  ]
 ]
 count-new-boxes
 set explore? false
end

to count-new-boxes
 set old-box-count new-box-count
 set new-box-count count boxes
 ifelse old-box-count = new-box-count
 [set any-new-boxes? false]
 [set any-new-boxes?  true]
end

to update-past-present-future-states
 ask boxes [
  if present? = true
  [set past? true set present? false]
  if future?   = true
  [set future? false set present? true]
  ]
end

to calculate-box-counting-dimension

  if count boxes >= 1 [     ; eliminates potential error message if setup is pressed during a box-counting procedure
   set-current-plot "Box Counting Plot"
   set-current-plot-pen "default"
   let no-boxes log (count boxes ) 10
   let scale (log ( 1 / box-size ) 10 )
   plotxy scale no-boxes
   set x-axis-list lput scale x-axis-list
   set y-axis-list lput no-boxes y-axis-list
   ]

 stop
end

to fractal-dimension
 if ticks = 0 [
  if divisions > 0 [ ; eliminates potential error message
  let line-segments count turtles
  set fractal-dim precision(( log  line-segments  10 / log  divisions 10 ))3
  ]
 ]
 stop
end

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;; Fractal Commands ;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; walk moves a turtle forward by a length (len) and leaves a trail of green patches.

to walk
  ask turtles with [not new?]
  [
    set pcolor green
    let walk-count .1
    while [walk-count <= len]
    [
      fd .1
      set pcolor green
      set walk-count ( walk-count + .1)
    ]
  ]
  stop
end

;hatch a new turtle for all the turtles that are not new to the current iteration and set new? to be true

to t
  ask turtles with [not new?] [hatch 1 [set new? true]]
end

;turn each turtle that wasn't created during the current iteration to the right by degree degrees
to r [degree]
  ask turtles with [not new?] [rt degree]
end

;turn each turtle that wasn't created during the current iteration to the left by degree degrees
to l [degree]
  ask turtles with [not new?] [lt degree]
end

;move each turtle that wasn't created during the current iteration forward by steps but do not leave trail of green patches.
to skip [steps]
  ask turtles with [not new?] [ fd steps ]
end

;all non-new turtles die
to d
  ask turtles with [not new?] [die]
end


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;;;;;;;;;Linear Reg;;;;;;;;;;;;;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

 ;Linear regression is used to find the 'best-fit' line
 ;through all the tuples (box-size,number of boxes) plotted in the scatter plot.
 ;The slope of the line is the box counting dimension.

 to linear-regression

  if (count boxes >= 1) and (length x-axis-list > 1) [  ; eliminates potential error message if setup is pressed during a box-counting procedure
                                                        ; or if there is only one point in the log-log plot

    let regression matrix:regress matrix:from-column-list (list y-axis-list  x-axis-list)   ;using the regression tool from the matrix extension
    ;setting y-intercept and slope (measure of goodness of fit)
    let y-intercept item 0 (item 0 regression)
    set slope item 1 (item 0 regression)
    set r-square item 0 (item 1 regression)


    ; set the equation to the appropriate string
    set lin-reg-eq (word (precision slope 2) " * x + " (precision y-intercept 2))

    ;plotting the line on the log-log plot
    set-current-plot "Box Counting Plot"
    set-current-plot-pen "pen-4"
    plot-pen-reset
    auto-plot-off
    plotxy plot-x-min (plot-x-min * slope + y-intercept)
    plot-pen-down
    plotxy plot-x-max (plot-x-max * slope + y-intercept)
    plot-pen-up
  ]
end
