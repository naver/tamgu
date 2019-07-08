/*
 *  Tamgu (탐구)
 *
 * Copyright 2019-present NAVER Corp.
 * under BSD 3-clause
 * This file can only be used with the Tamgu (탐구) library or the executable
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
 Project    : Tamgu (탐구)
 Version    : See tamgu.cxx for the version number
 filename   : ViewController.m
 Date       : 2017/09/01
 Purpose    : 
 Programmer : Claude ROUX (claude.roux@naverlabs.com)
 Reviewer   :
 */

#import "ViewController.h"
#import "AppDelegate.h"
ViewController* vue = nil;

extern AppDelegate* currentdelegate;

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    // Do any additional setup after loading the view.
    vue = self;
}


- (void)setRepresentedObject:(id)representedObject {
    [super setRepresentedObject:representedObject];

    // Update the view, if already loaded.
}

-(Console*)Maconsole {
    return _maconsole;
}


@end
