//
//  RootViewController.h
//  Infinitest
//
//  Created by Simon Peccaud on 3/20/12.
//  Copyright 2012 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>

#import "InfinitestData.h"

@interface RootViewController : UITableViewController <UIDocumentInteractionControllerDelegate> {
    IBOutlet UITableView    *oTableView;

    InfinitestData*         _data;
}

@end
